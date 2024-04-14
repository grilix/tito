#include <catch2/catch_test_macros.hpp>

#include <tito/Sensors.h>
#include <bitset>

TEST_CASE( "MemoryValueReader", "refreshValue" ) {
  int16_t valueSource = 0;

  tito::MemoryValueSource<int16_t> reader(&valueSource);

  REQUIRE( reader.getValue() == 0 );

  valueSource = 444;

  REQUIRE( reader.getValue() == 444 );
}

TEST_CASE( "AnalogToDigitalSensor", "refreshValue" ) {
  int16_t valueSource = 0;

  tito::MemoryValueSource<int16_t> mem(&valueSource);
  tito::AnalogToDigital reader(&mem);

  REQUIRE( reader.getValue() == false );

  valueSource = 800;

  REQUIRE( reader.getValue() == true );

  valueSource = 444;

  REQUIRE( reader.getValue() == false );
}

TEST_CASE( "ValueSources", "" ) {
  int16_t valueSource = 0;

  tito::MemoryValueSource<int16_t> source(&valueSource);

  REQUIRE( *source.getValue() == 0 );
  valueSource = 100;
  REQUIRE( *source.getValue() == 100 );

  tito::AnalogToDigital digital(&source);
  REQUIRE( *digital.getValue() == false );
  valueSource = 600;
  REQUIRE( *digital.getValue() == true );
}

TEST_CASE ( "digital check" ) {
  // digital check
  int16_t valueSource;
  std::bitset<3> source;

  valueSource = 1;
  REQUIRE( ((valueSource >> 0) & 0b0000'0001) == 1 );
  valueSource = 2;
  REQUIRE( ((valueSource >> 0) & 0b0000'0001) == 0 );
  REQUIRE( ((valueSource >> 1) & 0b0000'0001) == 1 );
  REQUIRE( ((valueSource >> 2) & 0b0000'0001) == 0 );

  // bits
  source = 1;
  REQUIRE ( source.test(0) == true );
  source = 2;
  REQUIRE ( source.test(0) == false );
  REQUIRE ( source.test(1) == true );
  REQUIRE ( source.test(2) == false );
  source = 152;
  REQUIRE ( source.test(0) == false );
  REQUIRE ( source.test(1) == false );
  REQUIRE ( source.test(2) == false );
}
