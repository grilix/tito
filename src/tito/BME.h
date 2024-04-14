#pragma once

#ifdef TITO_USE_ADAFRUIT_BME280
#define BME_VALUE_NOT_CHANGED 0x8000

#include <Adafruit_BME280.h>
#else
#error "Please declare a compatible BME device to use."
#endif

#include "TopicNode.h"
#include "Sensors.h"

namespace tito {
  template <typename T>
  class BME
  {
  public:
    BME(uint8_t addr, TopicNode* parent, const char* name) :
        _addr(addr),
        _topic(parent, name)
      { }

      const TopicNode* getTopic()
      {
        return &_topic;
      }

      MemoryValueSource<bool> getStateSensor()
      {
        return MemoryValueSource<bool>(&_started);
      }

      MemoryValueSource<float> getTemperatureSensor()
      {
        return MemoryValueSource<float>(&_temperatureValue);
      }

      MemoryValueSource<float> getHumiditySensor()
      {
        return MemoryValueSource<float>(&_humidityValue);
      }

      MemoryValueSource<float> getPressureSensor()
      {
        return MemoryValueSource<float>(&_pressureValue);
      }

      bool isStarted() { return _started; }

      void refreshValue();
      void setup();

    private:
      uint8_t _addr;
      TopicNode _topic;

      float _humidityValue    = 0;
      float _temperatureValue = 0;
      float _pressureValue    = 0;

      T _bme;

      bool _started = false;
  };

#ifdef TITO_USE_ADAFRUIT_BME280
  template<>
  void BME<Adafruit_BME280>::setup()
  {
    _started = _bme.begin(_addr);
  }

  template<>
  void BME<Adafruit_BME280>::refreshValue()
  {
    float newValue;

    if (!_started) {
      return;
    }

    if ((newValue = _bme.readTemperature()) != BME_VALUE_NOT_CHANGED) {
      this->_temperatureValue = newValue;
    }
    if ((newValue = _bme.readHumidity()) != BME_VALUE_NOT_CHANGED) {
      this->_humidityValue = newValue;
    }
    if ((newValue = _bme.readPressure()) != BME_VALUE_NOT_CHANGED) {
      this->_pressureValue = newValue / 100.F;
    }
  }
#endif
}
