#include <Arduino.h>

#define TITO_USE_ADAFRUIT_BME280

#define WIFI_SSID "<ssid>"
#define WIFI_PASSWORD "<pass>"
#define MQTT_HOST "<host>"
#define MQTT_USER "<user>"
#define MQTT_PASSWORD "<pass>"
#define MQTT_PORT 1883

#include <tito/Timer.h>
#include <tito/Sensors.h>
#include <tito/TopicNode.h>
#include <tito/GPIO.h>

#include <tito/BME.h>

#include <tito/ReportableGroup.h>
#include <tito/CommandableGroup.h>
#include <tito/Network.h>

#define MQTT_NAMESPACE "home/bme-test"
#define FAST_TIMER_TICK 500
#define SLOW_TIMER_TICK 20'000
#define LED 2

struct App {
  App(
    const char* name,
    const tito::NetworkCredentials& netCredentials,
    const tito::MQTTCredentials& mqttCredentials
  ) :
    fastTimer(FAST_TIMER_TICK),
    slowTimer(SLOW_TIMER_TICK),
    beatTimer(500), // slow blink when waiting for network
    fastBeatTimer(200), // fast blink when bme failed to start

    topic(name),

    bme(0x76, &topic, "bme"),
    bmeStatus(bme.getStateSensor()),
    bmeStatusReporter(&bmeStatus, bme.getTopic(), "chip"),

    temperatureSensor(bme.getTemperatureSensor()),
    humiditySensor(bme.getHumiditySensor()),
    pressureSensor(bme.getPressureSensor()),

    temperatureReporter(&temperatureSensor, bme.getTopic(), "temperature"),
    humidityReporter(&humiditySensor, bme.getTopic(), "humidity"),
    pressureReporter(&pressureSensor, bme.getTopic(), "pressure"),

    statusLed(gpio.getBitWriter(LED)),
    ledReporter(&statusLed, &topic, "led1"),
    ledCommander(&statusLed, ledReporter.getTopic()),

    commanders({
      &ledCommander
    }),

    network(&topic, netCredentials, mqttCredentials, &commanders),

    reporters(&network, {
      tito::SensorNode(&bmeStatusReporter, &slowTimer),
      tito::SensorNode(&temperatureReporter, &slowTimer),
      tito::SensorNode(&humidityReporter, &slowTimer),
      tito::SensorNode(&pressureReporter, &slowTimer),
      tito::SensorNode(&ledReporter, &fastTimer)
    })
  { }

  tito::Timer fastTimer;
  tito::Timer slowTimer;
  tito::Timer beatTimer;
  tito::Timer fastBeatTimer;
  tito::GPIO<3> gpio; // TODO: set N as needed

  tito::TopicNode topic;

  tito::BME<Adafruit_BME280> bme;
  tito::MemoryValueSource<bool> bmeStatus;
  tito::BooleanReporter bmeStatusReporter;

  tito::MemoryValueSource<float> temperatureSensor;
  tito::MemoryValueSource<float> humiditySensor;
  tito::MemoryValueSource<float> pressureSensor;

  tito::FloatReporter temperatureReporter;
  tito::FloatReporter humidityReporter;
  tito::FloatReporter pressureReporter;

  tito::BitWriter statusLed;
  tito::BooleanReporter ledReporter;
  tito::BooleanCommander ledCommander;

  tito::CommandableGroup<
    // TODO: set N as needed
    1 // led commander
  > commanders;

  tito::Network network;

  tito::ReportableGroup<
    // TODO: set N as needed
    1 + // bme status
    3 + // temp, humi, pres
    1   // led reporter
  > reporters;

  void setup()
  {
    gpio.setup();
    bme.setup();
    network.connect();
  }
};

tito::NetworkCredentials netCredentials = { WIFI_SSID, WIFI_PASSWORD };
tito::MQTTCredentials mqttCredentials = { MQTT_HOST, MQTT_PORT, MQTT_USER, MQTT_PASSWORD };

static App app = App(
  MQTT_NAMESPACE,
  netCredentials,
  mqttCredentials
);
unsigned long currentMillis = 0;

void setup()
{
  app.setup();
  app.statusLed.setValue(true);
}

void loop()
{
  currentMillis = millis();
  app.fastTimer.loop(currentMillis);
  app.slowTimer.loop(currentMillis);

  if (!app.network.isConnected()) {
    if (app.beatTimer.isTick(currentMillis)) {
      app.statusLed.toggleValue();
    }
    return;
  }

  if (!app.bme.isStarted()) {
    if (app.fastBeatTimer.isTick(currentMillis)) {
      app.statusLed.toggleValue();
    }
  } else {
    // if (*app.statusLed.getValue()) {
    //   // We'd normally turn the led off, but we'll keep it so it can be
    //   // controlled with commands (on/off) sent to "<namespace>/led1/command"
    //   app.statusLed.setValue(false);
    // }
    if (app.slowTimer.isTicking()) {
      app.bme.refreshValue();
    }
  }

  app.reporters.reportValues();
}
