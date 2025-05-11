#include "../entities/device.h"
#include "../entities/config.h"

#include <vector>

class IGlobal {
public:
    IGlobal() {};
    ~IGlobal() {};

    virtual bool init(Config config) = 0;  
    virtual bool clean() = 0;

    virtual std::vector<Device> getDevices() = 0;
    virtual Device getDefaultDevice() = 0;
};
