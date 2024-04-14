#pragma once

#ifdef TITO_USE_PCF_8574
#include <Adafruit_PCF8574.h>
#else
  #ifndef TITO_USE_PCF_DUMMY
  #error "Please declare a compatible pcf device to use."
  #endif
#endif

#include <array>
#include <bitset>

#include "Sensors.h"
#include "TopicNode.h"

#include "interfaces.h"

namespace tito {
  template <class T, size_t N>
  class PCFInterface;

  template <class T, size_t N>
  class PCF :
    public Refresheble,
    public BitSourceContainer,
    public BitWriterContainer
  {
    friend struct PCFInterface<T, N>;

  public:
    PCF(
        const uint8_t addr,
        const TopicNode* parent,
        const char* name
    ) :
        _topic(parent, name),
        _addr(addr)
    { }

    BitSource getBitSensor(const uint8_t index)
    {
      // pinModes[index] = INPUT;
      setInput(index);
      return BitSource(this, index);
    }

    BitWriter getBitWriter(const uint8_t index, const bool inverted)
    {
      // pinModes[index] = OUTPUT;
      setOutput(index);
      return BitWriter(this, index, inverted);
    }

    BitWriter getBitWriter(const uint8_t index)
    {
      return getBitWriter(index, false);
    }

    const TopicNode* getTopic() { return &_topic; }

    void setup();
    void refreshValue() override;
    bool readBit(const uint8_t index) override ;
    void writeBit(const uint8_t index, const bool value) override;
    void setInput(const uint8_t index);
    void setOutput(const uint8_t index);

  private:
    std::array<std::optional<uint8_t>, N> pinModes{};
    uint16_t _reading = 0;

    TopicNode _topic;
    const uint8_t _addr;

    T _pcf;
    bool _started = false;
  };

#ifdef TITO_USE_PCF_8574
  template <size_t N>
  class PCFInterface<Adafruit_PCF8574, N>
  {
    public:
    static void setup(PCF<Adafruit_PCF8574, N>* pcf)
    {
      pcf->_started = pcf->_pcf.begin(pcf->_addr, &Wire);
      for (size_t i = 0; i < N; i++) {
        if (pcf->pinModes[i]) {
          pcf->_pcf.pinMode(i, *pcf->pinModes[i]);
        }
      }
    }

    static void refreshValue(PCF<Adafruit_PCF8574, N>* pcf)
    {
      pcf->_reading = (uint16_t)pcf->_pcf.digitalReadByte();
    }

    static bool readPin(PCF<Adafruit_PCF8574, N>* pcf, const uint8_t index)
    {
      return (pcf->_reading >> index) & 0x1;
    }

    static void writePin(PCF<Adafruit_PCF8574, N>* pcf, const uint8_t index, const bool value)
    {
      (void)pcf->_pcf.digitalWrite(index, value);
    }

    static void setInput(PCF<Adafruit_PCF8574, N>* pcf, const uint8_t index)
    {
      pcf->pinModes[index] = INPUT;
    }

    static void setOutput(PCF<Adafruit_PCF8574, N>* pcf, const uint8_t index)
    {
      pcf->pinModes[index] = OUTPUT;
    }
  };
#endif

  template <class T, size_t N>
  void PCF<T, N>::setup()
  {
    PCFInterface<T, N>::setup(this);
  }

  template <class T, size_t N>
  void PCF<T, N>::refreshValue()
  {
    if (!_started) {
      return;
    }

    PCFInterface<T, N>::refreshValue(this);
  }

  template <class T, size_t N>
  bool PCF<T, N>::readBit(const uint8_t index)
  {
    return PCFInterface<T, N>::readPin(this, index);
  }

  template <class T, size_t N>
  void PCF<T, N>::writeBit(const uint8_t index, const bool value)
  {
    PCFInterface<T, N>::writePin(this, index, value);
  }

  template <class T, size_t N>
  void PCF<T, N>::setInput(const uint8_t index)
  {
    PCFInterface<T, N>::setInput(this, index);
  }

  template <class T, size_t N>
  void PCF<T, N>::setOutput(const uint8_t index)
  {
    PCFInterface<T, N>::setOutput(this, index);
  }
}
