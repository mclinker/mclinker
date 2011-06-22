

namespace mcld {

  class Target {
  friend struct TargetRegister;
  public:  
    typedef LDBackend  *(*LDBackendCtorTy)();
    typedef MCLDDriver *(*LDDriverCtorTy)();


  private:
    LDBackend LDBackendCtorFn;
    LDDriverCtorTy LDDriverCtorFn;
    


  public:
    LDBackend *createLDBackend(/*Constructor param*/) {
      if (LDBackendCtorFn)
        return LDBackendCtorFn(/*Constructor param*/);
    }

    MCLDDriver *createLDDriver(/*Constructor param*/) {
      if (LDDriverCtorFn){
        return LDDriverCtorFn(/*Constructor param*/);
      }  

  };

  struct TargetRegistry {
    
    static void RegistryLDBackend(Target &T, LDBackendTy Fn) {
      if (!T.LDBackendCtorFn)
        T.LDBackendCtorFn = Fn;
    }

    static void RegisterMCLDDriver(Target &T, 
                                   Target::LDDriverCtorTy Fn) {
      if (!T.LDDriverCtorFn)
        T.LDDriverCtorFn = Fn;
    }

  };

} //end namespace mcld
