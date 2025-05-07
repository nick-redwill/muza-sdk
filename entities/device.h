#ifndef DEVICE_H
#define DEVICE_H

#include <string>
#include <stdint.h>

struct Device {
    Device(int32_t id, std::string name) : id(id), name(name) {};

    int32_t id;
    std::string name;
};

#endif
