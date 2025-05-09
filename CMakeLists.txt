cmake_minimum_required(VERSION 3.11)
project(muza-sdk)

set(CMAKE_CXX_STANDARD 11)

enable_testing()

add_subdirectory(external/googletest)

include_directories(
    backend/bass
    cli
    entities
    interfaces
    utils
    
    lib/bass/inc
)

set(SOURCES
    backends/bass/BassGlobal.cpp
    backends/bass/BassStream.cpp
    cli/player.cpp
)

add_executable(muza_cli ${SOURCES})
target_link_libraries(muza_cli bass)

add_executable(muza_tests
    tests/bass/test_stream.cpp

    backends/bass/BassGlobal.cpp
    backends/bass/BassStream.cpp
)

target_link_libraries(muza_tests
    gtest_main
    bass
)

include(GoogleTest)
gtest_discover_tests(muza_tests)
