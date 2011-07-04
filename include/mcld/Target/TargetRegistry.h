/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@mediatek.com>                                         *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#ifndef TARGET_REGISTRY_H
#define TARGET_REGISTRY_H
#include <llvm/Target/TargetRegistry.h>
#include <list>

namespace llvm {
class Target;
class MCCodeEmitter;
class MCContext;
} // namespace of llvm

namespace mcld {

class TargetLDBackend;
class LLVMTargetMachine;
class TargetRegistry;
class MCLDDriver;

//===----------------------------------------------------------------------===//
/// Target - mcld::Target is an object adapter of llvm::Target
///
class Target 
{
  friend class mcld::LLVMTargetMachine;
  friend class mcld::TargetRegistry;
public:  
  typedef TargetLDBackend  *(*TargetLDBackendCtorTy)();

private:
  TargetLDBackendCtorTy TargetLDBackendCtorFn;

public:
  Target();

  void setTarget(const llvm::Target& pTarget) {
    m_pT = &pTarget;
  }

  TargetLDBackend *createLDBackend() const {
    if (!TargetLDBackendCtorFn)
      return 0;
    return TargetLDBackendCtorFn();
  }

  const llvm::Target* get() const {
    return m_pT;
  }

private:
  const llvm::Target* m_pT;
};

//===----------------------------------------------------------------------===//
/// TargetRegistry - mcld::TargetRegistry is an object adapter of 
/// llvm::TargetRegistry
///
class TargetRegistry 
{
public:
  typedef std::list<mcld::Target*> TargetListTy;

private:
  static TargetListTy s_TargetList;

public:
  static void RegisterTargetLDBackend(mcld::Target &T, mcld::Target::TargetLDBackendCtorTy Fn) {
    if (!T.TargetLDBackendCtorFn)
      T.TargetLDBackendCtorFn = Fn;
  }

  /// RegisterTarget - Register the given target. Attempts to register a
  /// target which has already been registered will be ignored.
  ///
  /// Clients are responsible for ensuring that registration doesn't occur
  /// while another thread is attempting to access the registry. Typically
  /// this is done by initializing all targets at program startup.
  ///
  /// @param T - The target being registered.
  static void RegisterTarget(mcld::Target &T);

  /// lookupTarget - Lookup a target based on a llvm::Target.
  ///
  /// @param T - The llvm::Target to find
  static const mcld::Target *lookupTarget(const llvm::Target& T);
};

/// RegisterTarget - Helper function for registering a target, for use in the
/// target's initialization function. Usage:
///
/// Target TheFooTarget; // The global target instance.
///
/// extern "C" void LLVMInitializeFooTargetInfo() {
///   RegisterTarget X(TheFooTarget, "foo", "Foo description");
/// }
struct RegisterTarget 
{
  RegisterTarget(mcld::Target &T, const char *Name) {
    llvm::TargetRegistry::iterator TIter, TEnd = llvm::TargetRegistry::end();
    // lookup llvm::Target
    for( TIter=llvm::TargetRegistry::begin(); TIter!=TEnd; ++TIter ) {
      if( 0==strcmp(TIter->getName(), Name) )
        break;
    }
    T.setTarget(*TIter);

    TargetRegistry::RegisterTarget(T);
  }
};


/// RegisterLDBackend - Helper template for registering a target specific
/// linker backend, for use in the target machine initialization function. 
/// Usage:
///
/// extern "C" void LLVMInitializeFooLDBackend() {
///   extern Target TheFooTarget;
///   RegisterLDBackend<FooSectLinker> X(TheFooTarget);
/// }
template<class LDBackendImpl>
struct RegisterLDBackend
{
  RegisterLDBackend(mcld::Target &T) {
    mcld::TargetRegistry::RegisterTargetLDBackend(T, &Allocator);
  }

private:
  static TargetLDBackend *Allocator() {
    return new LDBackendImpl();
  }
};

} //end namespace mcld

#endif

