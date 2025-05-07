#include <bass.h>

#include "BassGlobal.h"

bool BassGlobal::init(Config config) {
    uint32_t flags = BASS_DEVICE_STEREO ? config.stereo : BASS_DEVICE_MONO;

    return BASS_Init(config.device.id, config.sampleRate, flags, 0, 0);
}

std::vector<Device> BassGlobal::getDevices() {
    std::vector<Device> devices;

    BASS_DEVICEINFO info;
    for (int a = 1; BASS_GetDeviceInfo(a, &info); a++)
        if (info.flags & BASS_DEVICE_ENABLED) // Listing only enabled devices
	{
	    devices.push_back(Device(a, std::string(info.name)));
	}
    
    return devices;
}

Device BassGlobal::getDefaultDevice() {
    return Device(-1, "Default Device");
}

bool BassGlobal::clean() {
    return BASS_Free();
}
