#pragma once

#include <optional>
#include <cstdint>
#include <cstdio>
#include <array>

#include "TopicNode.h"

#include "interfaces.h"

namespace tito {
  class BitSource : public ValueSource<bool> {
    public:
      BitSource(BitSourceContainer* hub, const uint8_t index) :
        _hub(hub),
        _index(index)
      { }

      std::optional<bool> getValue() override
      {
        return _hub->readBit(_index);
      }

    private:
      BitSourceContainer* _hub;
      const uint8_t _index;
  };

  class BitWriter :
    public ValueDestination<bool>,
    public ValueSource<bool>
  {
    public:
      BitWriter(BitWriterContainer* hub, const uint8_t index, const bool inverted) :
        _hub(hub),
        _index(index),
        _inverted(inverted)
      {}

      BitWriter(BitWriterContainer* hub, const uint8_t index) :
        BitWriter(hub, index, false)
      {}

      void setValue(const bool value) override
      {
        _value = value;
        _hub->writeBit(_index, _inverted xor *_value);
      }

      void toggleValue()
      {
        _value = !*_value;
        _hub->writeBit(_index, _inverted xor *_value);
      }

      std::optional<bool> getValue() override
      {
        return _value;
      }

    private:
      BitWriterContainer* _hub;
      const uint8_t _index;
      std::optional<bool> _value;
      const bool _inverted;
  };

  class BooleanReporter : public Reportable {
    public:
      BooleanReporter(ValueSource<bool>* source, const TopicNode* parent, const char* name) :
        _topic(parent, name),
        _source(source)
      { }

      void reportValue(ReportReceiver* reporter) override
      {
        std::optional<bool> value = _source->getValue();
        if (!value) {
          return;
        }

        if (oldValue && (*value == *oldValue)) {
          return;
        }
        oldValue = value;

        if (*value) {
          reporter->onValue(&_topic, "on", 2);
        } else {
          reporter->onValue(&_topic, "off", 3);
        }
      }

      const TopicNode* getTopic() { return &_topic; };

    private:
      const TopicNode _topic;
      ValueSource<bool>* _source;
      std::optional<bool> oldValue;
  };

  class BooleanCommander : public Commandable {
    public:
      BooleanCommander(
        ValueDestination<bool>* destination,
        const TopicNode* parent // TODO: sensor topic
      ) :
        _topic(parent, "command"),
        _destination(destination)
      {}

      void parseCommand(const char* value, const size_t valueLen) override
      {
        if (valueLen == 2 && strncmp(value, "on", valueLen) == 0) {
          _destination->setValue(true);
        } else if (valueLen == 3 && strncmp(value, "off", valueLen) == 0) {
          _destination->setValue(false);
        } else {
          // Serial.print("unknown command: ");
          // Serial.println(value);
          // Serial.print("command len: ");
          // Serial.println(valueLen);
        }
      }

      bool topicMatches(const char* topic, const size_t topicLen) override
      {
        return _topic.topicMatches(topic, topicLen);
      }

    private:
      TopicNode _topic;
      ValueDestination<bool>* _destination;
  };

  class FloatReporter : public Reportable {
    public:
      FloatReporter(ValueSource<float>* source, const TopicNode* parent, const char* name) :
        _topic(parent, name),
        _source(source)
      { }

      void reportValue(ReportReceiver* reporter) override
      {
        char valueBuffer[12];
        std::optional<float> value = _source->getValue();
        if (!value) {
          return;
        }

        // TODO: threshold?
        if (oldValue && (*value == *oldValue)) {
          return;
        }
        oldValue = value;

        size_t result = snprintf(valueBuffer, 12, "%.2f", *value);
        if (result >= 12) {
          result = (12 - 1);
        }

        reporter->onValue(&_topic, valueBuffer, result);
      }

    private:
      TopicNode _topic;
      ValueSource<float>* _source;
      std::optional<float> oldValue;
  };

  template <typename T>
  class MemoryValueSource : public ValueSource<T> {
    public:
      explicit MemoryValueSource(const T* address) :
        _address(address)
      { }

      std::optional<T> getValue() override
      {
        return *_address;
      }

    private:
      const T* _address;
  };

  class AnalogToDigital : public ValueSource<bool> {
    public:
      explicit AnalogToDigital(ValueSource<int16_t>* source) :
        _source(source)
      { }

      std::optional<bool> getValue() override
      {
        std::optional<int16_t> value = _source->getValue();
        if (!value) {
          return {};
        }

        return *value >= 500;
      }

    private:
      ValueSource<int16_t>* _source;
  };

  template <typename T>
  class SensorWithValue  {
    public:
      virtual std::optional<T> getValue() { return currentValue; };

    protected:
      std::optional<T> currentValue{};
  };
};
