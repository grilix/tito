#pragma once

#include <Arduino.h>
#include "Sensors.h"

namespace tito {
  template <size_t N>
  class GPIO : public BitSourceContainer, public BitWriterContainer {
    public:

      BitSource getBitSensor(const uint8_t index)
      {
        pinModes[index] = INPUT;
        return BitSource(this, index);
      }

      BitWriter getBitWriter(const uint8_t index, const bool inverted)
      {
        pinModes[index] = OUTPUT;
        return BitWriter(this, index, inverted);
      }

      BitWriter getBitWriter(const uint8_t index)
      {
        return getBitWriter(index, false);
      }

      void setup()
      {
        for (size_t i = 0; i < N; i++) {
          if (pinModes[i]) {
            pinMode(i, *pinModes[i]);
          }
        }
      }

      bool readBit(const uint8_t index) override
      {
        return digitalRead(index) == HIGH;
      }

      void writeBit(const uint8_t index, const bool value) override
      {
        digitalWrite(index, value ? LOW : HIGH);
      }

    private:
      std::array<std::optional<uint8_t>, N> pinModes{};
  };
}
