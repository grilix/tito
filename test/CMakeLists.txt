cmake_minimum_required(VERSION 3.15.0)

add_compile_options(-Wall -Og -g3 -ggdb)

add_executable(Test
  test_TopicNode/test_main.cpp
  test_Timer/test_main.cpp
  test_Sensors/test_main.cpp
  test_PCF/test_main.cpp
  test_MCP/test_main.cpp
)

Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

target_link_libraries(Test
  PRIVATE
    Catch2::Catch2WithMain
)

include_directories(Test PRIVATE
                          "${PROJECT_SOURCE_DIR}/src"
                          )
