#pragma once

#include "interfaces.h"

namespace tito {
  class SensorNode {
    public:
      SensorNode(
        Reportable* sensor,
        Timer* timer
      ) :
        _sensor(sensor),
        _timer(timer)
      { }

      void reportNodeValue(ReportReceiver* reporter)
      {
        if (!_timer->isTicking()) {
          return;
        }

        _sensor->reportValue(reporter);
      }

    private:
      Reportable* _sensor;
      Timer* _timer;
  };

  template <size_t N>
  struct ReportableGroup {
    ReportReceiver* _reporter;
    std::array<SensorNode, N> _values;

    ReportableGroup(
      ReportReceiver* reporter,
      std::array<SensorNode, N> configs
    ) :
      _reporter(reporter),
      _values(configs)
    { }

    void reportValues()
    {
      for (size_t i = 0; i < N; i++) {
        _values[i].reportNodeValue(_reporter);
      }
    }
  };
}
