#include "../interfaces/IGlobal.h"

class BassGlobal : public IGlobal {

public:
   virtual bool init() override;
   virtual bool clean() override;
};
