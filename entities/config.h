#ifndef CONFIG_H
#define CONFIG_H

#include "device.h"

struct Config {
    Config(Device device, uint32_t sampleRate=44100, bool stereo=false)
        : device(device), sampleRate(sampleRate), stereo(stereo) {}; 

    Device device;
    uint32_t sampleRate;
    bool stereo; //TODO: Maybe turn into an enum
};
#endif
