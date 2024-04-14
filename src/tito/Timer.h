#pragma once

namespace tito {
  struct Timer {
    const unsigned int _tickTime;
    unsigned long nextTick = 0;
    bool ticking = false;

    explicit Timer(unsigned int tickTime) :
      _tickTime(tickTime)
    { }

    bool isTick(unsigned long currentMillis) {
      ticking = wouldTick(currentMillis);

      if (ticking) {
        tick(currentMillis);
      }

      return ticking;
    }

    bool wouldTick(unsigned long currentMillis) {
      return currentMillis >= nextTick;
    }

    void tick(unsigned long currentMillis) {
      nextTick = currentMillis + _tickTime;
    }

    void loop(unsigned long currentMillis) {
      if (isTick(currentMillis)) {
        tick(currentMillis);
      } else {
        ticking = false;
      }
    }

    bool isTicking() {
      return ticking;
    }
  };
};
