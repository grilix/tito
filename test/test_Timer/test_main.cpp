#include <catch2/catch_test_macros.hpp>

#include <tito/Timer.h>

TEST_CASE( "Timer", "isTick" ) {
  tito::Timer timer(100);
  // First is a tick
  REQUIRE( timer.isTick(50) == true );
  REQUIRE( timer.nextTick == 150 );
  // Doesn't tick
  REQUIRE( timer.isTick(100) == false );
  REQUIRE( timer.nextTick == 150 );
  // Does tick
  REQUIRE( timer.isTick(150) == true );
  REQUIRE( timer.nextTick == 250 );
}
