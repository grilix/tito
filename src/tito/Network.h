#pragma once

#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif

#include <Ticker.h>
#include <AsyncMqttClient.h>

#include "TopicNode.h"

#include "interfaces.h"

#define TITO_TOPIC_MAX 55
#define TITO_STATE_TOPIC "state"
#define TITO_NETWORK_NAME "network"

namespace tito {
  struct NetworkCredentials {
    const char* ssid;
    const char* password;

    NetworkCredentials(const char* _ssid, const char* _password) :
      ssid(_ssid),
      password(_password)
    { }
  };

  struct MQTTCredentials {
    const char* host;
    int port; // default: 1883;
    const char* user;
    const char* password;

    MQTTCredentials(const char* _host, int _port, const char* _user, const char* _password) :
      host(_host),
      port(_port),
      user(_user),
      password(_password)
    { }
  };

  class Network : public ReportReceiver { // TODO: name
    public:
      AsyncMqttClient mqttClient;

      Network(
        const TopicNode* topic,
        const NetworkCredentials& netCredentials,
        const MQTTCredentials& mqttCredentials,
        CommandRouter* commandRouter
      ) :
        _netCredentials(netCredentials),
        _mqttCredentials(mqttCredentials),
        _parent(topic),
        _topic(topic, TITO_NETWORK_NAME),
        _commandRouter(commandRouter)
      {
        char stateTopic[TITO_TOPIC_MAX];
        size_t topicLen = _topic.buildTopic(stateTopic, TITO_TOPIC_MAX, "state");
        if (topicLen != 0) {
          memcpy(_willTopic, stateTopic, topicLen + 1);
        }
      }

      void connect()
      {
        wifiConnectHandler = WiFi.onStationModeGotIP([=] (const WiFiEventStationModeGotIP& e) {
          Serial.println("WiFi connected");
          connectToMqtt();
        });

        wifiDisconnectHandler = WiFi.onStationModeDisconnected([=] (const WiFiEventStationModeDisconnected& e) {
          Serial.println("WiFi disconnected");
          mqttReconnectTimer.detach();
          this->mqttConnected = false;

          wifiReconnectTimer.once(2, [=]() {
            connectToWifi();
          });
        });

        mqttClient.onConnect([=] (bool sessionPresent) {
          Serial.println("MQTT connected");

          char stateTopic[TITO_TOPIC_MAX];

          if (_topic.buildTopic(stateTopic, TITO_TOPIC_MAX, "state") != 0) {
            mqttClient.publish(stateTopic, 0, true, "online", 6);
          }

          if (_parent->buildTopic(stateTopic, TITO_TOPIC_MAX, "#") != 0) {
            mqttClient.subscribe(stateTopic, 0);
          }

          this->mqttConnected = true;
        });

        mqttClient.onMessage([=] (
          const char* topic,
          const char* payload,
          AsyncMqttClientMessageProperties properties,
          size_t len,
          size_t index,
          size_t total
        ) {
          size_t topicLen = strlen(topic); // TODO: do we receive the length?

          if (topicLen <= 8) {
            return;
          }
          if (strncmp(topic + (topicLen - 8), "/command", 8) != 0) {
            return;
          }

          _commandRouter->routeCommand(topic, topicLen, payload, len);

          // Serial.println("Publish received.");
          // Serial.print("  topic: ");
          // Serial.println(topic);
          // Serial.print("  qos: ");
          // Serial.println(properties.qos);
          // Serial.print("  dup: ");
          // Serial.println(properties.dup);
          // Serial.print("  retain: ");
          // Serial.println(properties.retain);
          // Serial.print("  len: ");
          // Serial.println(len);
          // Serial.print("  index: ");
          // Serial.println(index);
          // Serial.print("  total: ");
          // Serial.println(total);
        });

        mqttClient.onDisconnect([=] (AsyncMqttClientDisconnectReason reason) {
          this->mqttConnected = false;
          onMqttDisconnect(reason);
        });

        mqttClient.setServer(_mqttCredentials.host, _mqttCredentials.port);

        if (_mqttCredentials.user != nullptr) {
          mqttClient.setCredentials(_mqttCredentials.user, _mqttCredentials.password);
        }

        connectToWifi();
      }

      bool isConnected() const
      {
        // If mqtt is connected, we're good.
        return this->mqttConnected;
      }

      void onValue(const TopicBuilder* topic, const char* valueBuffer, size_t valueLen) override
      {
        if (!this->mqttConnected) {
          return;
        }

        char topicBuffer[TITO_MAX_TOPIC_LEN];

        if (topic->buildTopic(topicBuffer, TITO_MAX_TOPIC_LEN, TITO_STATE_TOPIC) == 0) {
          return;
        }

        mqttClient.publish(topicBuffer, 0, false, valueBuffer, valueLen);
      }

    private:
      const NetworkCredentials _netCredentials;
      const MQTTCredentials _mqttCredentials;

      const TopicBuilder* _parent;
      const TopicNode _topic;
      CommandRouter* _commandRouter;

      char _willTopic[TITO_MAX_TOPIC_LEN];

      bool mqttConnected = false;

      Ticker mqttReconnectTimer;
      Ticker wifiReconnectTimer;

      WiFiEventHandler wifiConnectHandler;
      WiFiEventHandler wifiDisconnectHandler;

      void connectToWifi()
      {
        if (_netCredentials.ssid == nullptr || _mqttCredentials.host == nullptr) {
          return;
        }

        WiFi.begin(_netCredentials.ssid, _netCredentials.password);
      }

      void connectToMqtt()
      {
        mqttClient.setWill(_willTopic, 0, true, "offline", 7);
        mqttClient.connect();
      }

      void onMqttDisconnect(AsyncMqttClientDisconnectReason)
      {
        if (WiFi.isConnected()) {
          mqttReconnectTimer.once(2, [=]() {
            connectToMqtt();
          });
        }
      }
  };
};
