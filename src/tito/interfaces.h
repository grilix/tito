#pragma once

#include <optional>

namespace tito {
  template <typename T>
  class ValueDestination {
    public:
      virtual void setValue(T value) = 0;
  };

  class Commandable {
    public:
      virtual void parseCommand(const char* value, const size_t valueLen) = 0;
      virtual bool topicMatches(const char* topic, const size_t topicLen) = 0;
  };

  template <typename T>
  class ValueSource {
    public:
      virtual std::optional<T> getValue() = 0;
  };

  class BitSourceContainer {
    public:
      virtual bool readBit(const uint8_t index) = 0;
  };

  class BitWriterContainer {
    public:
      virtual void writeBit(const uint8_t index, const bool value) = 0;
  };

  class CommandRouter {
    public:
      virtual void routeCommand(
        const char* topic, const size_t topicLen,
        const char* command, const size_t commandLen
      ) = 0;
  };

  class TopicBuilder {
    public:
      virtual size_t buildTopic(char buffer[], const size_t maxLen, const char* topic) const = 0;
  };

  class ReportReceiver {
    public:
      virtual void onValue(const TopicBuilder* topic, const char* value, const size_t valueLen) = 0;
  };

  class Refresheble {
    public:
      virtual void refreshValue() = 0;
  };

  class Reportable {
    public:
      virtual void reportValue(ReportReceiver* reporter) = 0;
  };
}
