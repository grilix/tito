#pragma once

#include "interfaces.h"

namespace tito {
  template <size_t N>
  class CommandableGroup : public CommandRouter {
    public:
      explicit CommandableGroup(std::array<Commandable*, N> commanders) :
        _commanders(commanders)
      { }

      void routeCommand(
          const char* topic, const size_t topicLen,
          const char* command, const size_t commandLen
      ) override {
        for (auto commander: _commanders) {
          if (commander->topicMatches(topic, topicLen)) {
            commander->parseCommand(command, commandLen);
            break; // TODO: are we gonna have duplicates?
          }
        }
      }
    private:
      std::array<Commandable*, N> _commanders;
  };
}
