cmake_minimum_required(VERSION 3.15.0)

project(BASE LANGUAGES CXX VERSION 0.1.0)

option(PACKAGE_TESTS "Build the tests" ON)

add_subdirectory(src)

if(PACKAGE_TESTS)
  # message(${PROJECT_SOURCE_DIR})
  # message(${CMAKE_CURRENT_BINARY_DIR})

    include(CTest)
    enable_testing()
    add_subdirectory(test)
    add_test(NAME Test COMMAND Test)
endif()
