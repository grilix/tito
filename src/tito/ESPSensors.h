#pragma once

#include "Sensors.h"

namespace tito {
  class ESPVCCSensor : public ValueSource<float> {
    public:
      std::optional<float> getValue() override
      {
        return (float)ESP.getVcc();
      }
  };
};
