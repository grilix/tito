#include <catch2/catch_test_macros.hpp>

#define TITO_USE_MCP_DUMMY

#include <tito/MCP.h>

class DummyMCP {
  //
};

namespace tito {
  template <>
  class MCPInterface<DummyMCP, 2> {
    public:
      static void setup()
      {
      }

      static void refreshValue(MCP<DummyMCP, 2>* mcp)
      {
        // return true;
      }
  };
}

// class DummyMCPReporter : public tito::NodeReporter {
//   public:
//     void onValue(tito::TopicNode* topic, const char* value, size_t valueLen)
//     {
//       //
//     }
// };

TEST_CASE("MCP") {
  tito::TopicNode root("root");
  // DummyMCPReporter reporter;
  tito::MCP<DummyMCP, 2> mcp(3, &root, "mcp", {0, 1});
  //
}
