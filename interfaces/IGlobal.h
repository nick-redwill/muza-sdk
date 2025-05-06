class IGlobal {
public:
   IGlobal() {};
   ~IGlobal() {};

   enum class Callback {
      ON_INIT,
      ON_CLEAN,
      ON_ERROR   
   };

   virtual bool init() = 0; //TODO: Add arguments for initialization
   virtual bool clean() = 0;
};
