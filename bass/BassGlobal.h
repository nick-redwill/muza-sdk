#include "../interfaces/IGlobal.h"

class BassGlobal : public IGlobal {

public:
   virtual std::vector<Device> getDevices() override;
   virtual Device getDefaultDevice() override;

   virtual bool init(Config config) override;
   virtual bool clean() override;
};
