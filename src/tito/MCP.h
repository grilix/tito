#pragma once

#ifdef TITO_USE_MCP_ADC_3008
#include <MCP_ADC.h>
#else
  #ifndef TITO_USE_MCP_DUMMY
  #error "Please declare a compatible mcp device to use."
  #endif
#endif

#include "TopicNode.h"
#include "Sensors.h"

#include "interfaces.h"


namespace tito {
  template <typename T, size_t N>
  class MCPInterface;

  template <class T, size_t N>
  class MCP : public Refresheble {

    friend struct MCPInterface<T, N>;

    public:
      MCP(int pin, const TopicNode* parent, const char* name, std::array<uint8_t, N> channels) :
        _pin(pin),
        _topic(parent, name),
        _channels(channels)
      { }

      MemoryValueSource<int16_t> getSensor(size_t i)
      {
        return MemoryValueSource<int16_t>(&_readings[i]);
      }

      const TopicNode* getTopic() { return &_topic; }

      void setup();
      void refreshValue();

    private:
      int _pin;
      TopicNode _topic;

      int16_t _readings[N] = {0};
      std::array<uint8_t, N> _channels;

      T _mcp;
  };

#ifdef TITO_USE_MCP_ADC_3008
  template<size_t N>
  class MCPInterface<MCP3008, N>
  {
    public:
    static void setup(MCP<MCP3008, N> *mcp)
    {
      SPI.begin();
      mcp->_mcp.begin(mcp->_pin);
    }

    static void refreshValue(MCP<MCP3008, N> *mcp)
    {
      mcp->_mcp.readMultiple(mcp->_channels.data(), N, mcp->_readings);
    }
  };
#endif

  template <typename T, size_t N>
  void MCP<T, N>::setup()
  {
    MCPInterface<T, N>::setup(this);
  }

  template <typename T, size_t N>
  void MCP<T, N>::refreshValue()
  {
    MCPInterface<T, N>::refreshValue(this);
  }
}
