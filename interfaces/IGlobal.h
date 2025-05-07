class IGlobal {
public:
   IGlobal() {};
   ~IGlobal() {};
   
   virtual bool init() = 0; //TODO: Add arguments for initialization
   virtual bool clean() = 0;
};
