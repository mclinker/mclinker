

namespace mcld {

  class Target {
  friend struct TargetRegister;
  public:  
    typedef MCLDDriver *(*LDDriverCtorTy)();


  private:
    LDDriverCtorTy LDDriverCtorFn;


  public:
    MCLDDriver *createLDDriver(/*Constructor param*/) {
      if (LDDriverCtorFn){
        return LDDriverCtorFn(/*Constructor param*/);
      }  

  };

  struct TargetRegistry {
    static void RegisterMCLDDriver(Target &T, 
                                   Target::LDDriverCtorTy Fn) {
      if (!T.LDDriverCtorFn)
        T.LDDriverCtorFn = Fn;
    }

  };

} //end namespace mcld
