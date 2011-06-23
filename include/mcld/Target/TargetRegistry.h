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
  typedef MCLDDriver       *(*MCLDDriverCtorTy)();

private:
  TargetLDBackendCtorTy TargetLDBackendCtorFn;
  MCLDDriverCtorTy MCLDDriverCtorFn;

private:
  explicit Target(const llvm::Target& pTarget);
  ~Target();

public:
  TargetLDBackend *createLDBackend() const {
    if (!TargetLDBackendCtorFn)
      return 0;
    return TargetLDBackendCtorFn();
  }

  MCLDDriver *createLDDriver() const {
    if (!MCLDDriverCtorFn)
      return 0;
    return MCLDDriverCtorFn();
  }  

  /// createCodeEmitter - Create a target specific code emitter.
  llvm::MCCodeEmitter *createCodeEmitter(mcld::LLVMTargetMachine &TM,
                                         llvm::MCContext &Ctx) const; 

private:
  const llvm::Target& m_T;
};

//===----------------------------------------------------------------------===//
/// TargetRegistry - mcld::TargetRegistry is an object adapter of 
/// llvm::TargetRegistry
///
class TargetRegistry 
{
public:
  static void RegistryTargetLDBackend(mcld::Target &T, mcld::Target::TargetLDBackendCtorTy Fn) {
    if (0==T.TargetLDBackendCtorFn)
      T.TargetLDBackendCtorFn = Fn;
  }

  static void RegisterMCLDDriver(mcld::Target &T, mcld::Target::Target::MCLDDriverCtorTy Fn) {
    if (0==T.MCLDDriverCtorFn)
      T.MCLDDriverCtorFn = Fn;
  }
};

} //end namespace mcld

#endif

