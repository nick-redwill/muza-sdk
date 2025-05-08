cmake_minimum_required(VERSION 3.11)
project(muza-sdk)

set(CMAKE_CXX_STANDARD 11)

include_directories(
    bass
    cli
    entities
    interfaces
    utils
    lib
)

set(SOURCES
    bass/BassGlobal.cpp
    bass/BassStream.cpp
    cli/player.cpp
)

add_executable(audio_cli ${SOURCES})

if(WIN32)
    target_link_libraries(audio_cli bass)
elseif(UNIX)
    target_link_libraries(audio_cli bass)
endif()
