#include <catch2/catch_test_macros.hpp>

#define TITO_USE_PCF_DUMMY

#include <tito/PCF.h>

class DummyPCF { };

namespace tito {
  template <>
  class PCFInterface<DummyPCF, 2> {
    public:
      static void setup(PCF<DummyPCF, 2>* pcf)
      {
        pcf->_started = true;
      }

      static void refreshValue(PCF<DummyPCF, 2>* pcf)
      {
        pcf->_reading = (uint16_t)2;
      }

      static void setInput(PCF<DummyPCF, 2>* pcf, const uint8_t index)
      { }

      static void setOutput(PCF<DummyPCF, 2>* pcf, const uint8_t index)
      { }

      static bool readPin(PCF<DummyPCF, 2>* pcf, const uint8_t index)
      {
        return (pcf->_reading >> index) & 0x1;
      }

      static void writePin(PCF<DummyPCF, 2>* pcf, const uint8_t index, bool value)
      { }
  };
}

TEST_CASE("PCF") {
  tito::TopicNode root("root");
  tito::PCF<DummyPCF, 2> pcf(0x20, &root, "pcf");
  pcf.setup();

  auto source1 = pcf.getBitSensor(0);
  auto source2 = pcf.getBitSensor(1);

  pcf.refreshValue();

  REQUIRE( *source1.getValue() == false );
  REQUIRE( *source2.getValue() == true );
}
