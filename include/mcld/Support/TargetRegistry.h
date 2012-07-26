//===- TargetRegistry.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_REGISTRY_H
#define MCLD_TARGET_REGISTRY_H
#include <llvm/Support/TargetRegistry.h>
#include <string>
#include <list>

namespace llvm {
class TargetMachine;
class MCCodeEmitter;
class MCContext;
class AsmPrinter;
} // namespace of llvm

namespace mcld {
class LLVMTargetMachine;
class TargetRegistry;
class SectLinker;
class SectLinkerOption;
class TargetLDBackend;
class AttributeFactory;
class InputFactory;
class ContextFactory;
class DiagnosticLineInfo;

//===----------------------------------------------------------------------===//
/// Target - mcld::Target is an object adapter of llvm::Target
///
class Target
{
  friend class mcld::LLVMTargetMachine;
  friend class mcld::TargetRegistry;
public:
  typedef mcld::LLVMTargetMachine *(*TargetMachineCtorTy)(const mcld::Target &,
                                                          llvm::TargetMachine &,
                                                          const std::string&);

  typedef SectLinker *(*SectLinkerCtorTy)(const std::string& pTriple,
                                          SectLinkerOption &,
                                          TargetLDBackend&);

  typedef TargetLDBackend  *(*TargetLDBackendCtorTy)(const llvm::Target&,
                                                     const std::string&);

  typedef DiagnosticLineInfo *(*DiagnosticLineInfoCtorTy)(const mcld::Target&,
                                                          const std::string&);

public:
  Target();

  void setTarget(const llvm::Target& pTarget)
  { m_pT = &pTarget; }

  mcld::LLVMTargetMachine *createTargetMachine(const std::string &pTriple,
                          const std::string &pCPU, const std::string &pFeatures,
                          const llvm::TargetOptions &Options,
                          llvm::Reloc::Model RM = llvm::Reloc::Default,
                          llvm::CodeModel::Model CM = llvm::CodeModel::Default,
                          llvm::CodeGenOpt::Level OL = llvm::CodeGenOpt::Default) const
  {
    if (TargetMachineCtorFn && m_pT) {
      llvm::TargetMachine *tm = m_pT->createTargetMachine(pTriple, pCPU, pFeatures, Options, RM, CM, OL);
      if (tm)
        return TargetMachineCtorFn(*this, *tm, pTriple);
    }
    return NULL;
  }

  /// createSectLinker - create target-specific SectLinker
  ///
  /// @return created SectLinker
  SectLinker *createSectLinker(const std::string &pTriple,
                               SectLinkerOption &pOption,
                               TargetLDBackend &pLDBackend) const {
    if (!SectLinkerCtorFn)
      return NULL;
    return SectLinkerCtorFn(pTriple,
                            pOption,
                            pLDBackend);
  }

  /// createLDBackend - create target-specific LDBackend
  ///
  /// @return created TargetLDBackend
  TargetLDBackend* createLDBackend(const std::string& Triple) const
  {
    if (!TargetLDBackendCtorFn)
      return NULL;
    return TargetLDBackendCtorFn(*get(), Triple);
  }

  /// createDiagnosticLineInfo - create target-specific DiagnosticLineInfo
  DiagnosticLineInfo* createDiagnosticLineInfo(const mcld::Target& pTarget,
                                               const std::string& pTriple) const
  {
    if (!DiagnosticLineInfoCtorFn)
      return NULL;
    return DiagnosticLineInfoCtorFn(pTarget, pTriple);
  }

  const llvm::Target* get() const
  { return m_pT; }

private:
  // -----  function pointers  ----- //
  TargetMachineCtorTy TargetMachineCtorFn;
  SectLinkerCtorTy SectLinkerCtorFn;
  TargetLDBackendCtorTy TargetLDBackendCtorFn;
  DiagnosticLineInfoCtorTy DiagnosticLineInfoCtorFn;

  // -----  adapted llvm::Target  ----- //
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
  typedef TargetListTy::iterator iterator;

private:
  static TargetListTy s_TargetList;

public:
  static iterator begin() { return s_TargetList.begin(); }
  static iterator end() { return s_TargetList.end(); }

  static size_t size() { return s_TargetList.size(); }
  static bool empty() { return s_TargetList.empty(); }

  /// RegisterTarget - Register the given target. Attempts to register a
  /// target which has already been registered will be ignored.
  ///
  /// Clients are responsible for ensuring that registration doesn't occur
  /// while another thread is attempting to access the registry. Typically
  /// this is done by initializing all targets at program startup.
  ///
  /// @param T - The target being registered.
  static void RegisterTarget(mcld::Target &T);

  /// RegisterTargetMachine - Register a TargetMachine implementation for the
  /// given target.
  ///
  /// @param T - The target being registered.
  /// @param Fn - A function to construct a TargetMachine for the target.
  static void RegisterTargetMachine(mcld::Target &T, mcld::Target::TargetMachineCtorTy Fn)
  {
    // Ignore duplicate registration.
    if (!T.TargetMachineCtorFn)
      T.TargetMachineCtorFn = Fn;
  }

  /// RegisterSectLinker - Register a SectLinker implementation for the given
  /// target.
  ///
  /// @param T - the target being registered
  /// @param Fn - A function to create SectLinker for the target
  static void RegisterSectLinker(mcld::Target &T, mcld::Target::SectLinkerCtorTy Fn)
  {
    if (!T.SectLinkerCtorFn)
      T.SectLinkerCtorFn = Fn;
  }

  /// RegisterTargetLDBackend - Register a TargetLDBackend implementation for
  /// the given target.
  ///
  /// @param T - The target being registered
  /// @param Fn - A function to create TargetLDBackend for the target
  static void RegisterTargetLDBackend(mcld::Target &T, mcld::Target::TargetLDBackendCtorTy Fn)
  {
    if (!T.TargetLDBackendCtorFn)
      T.TargetLDBackendCtorFn = Fn;
  }

  /// RegisterTargetDiagnosticLineInfo - Register a DiagnosticLineInfo
  /// implementation for the given target.
  ///
  /// @param T - The target being registered
  /// @param Fn - A function to create DiagnosticLineInfo for the target
  static void
  RegisterDiagnosticLineInfo(mcld::Target &T,
                             mcld::Target::DiagnosticLineInfoCtorTy Fn)
  {
    if (!T.DiagnosticLineInfoCtorFn)
      T.DiagnosticLineInfoCtorFn = Fn;
  }

  /// lookupTarget - Lookup a target based on a llvm::Target.
  ///
  /// @param T - The llvm::Target to find
  static const mcld::Target *lookupTarget(const llvm::Target& T);

  /// lookupTarget - function wrapper of llvm::TargetRegistry::lookupTarget
  ///
  /// @param Triple - The Triple string
  /// @param Error  - The returned error message
  static const mcld::Target *lookupTarget(const std::string &Triple,
                                          std::string &Error);
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

/// RegisterTargetMachine - Helper template for registering a target machine
/// implementation, for use in the target machine initialization
/// function. Usage:
///
/// extern "C" void LLVMInitializeFooTarget() {
///   extern mcld::Target TheFooTarget;
///   RegisterTargetMachine<mcld::FooTargetMachine> X(TheFooTarget);
/// }
template<class TargetMachineImpl>
struct RegisterTargetMachine
{
  RegisterTargetMachine(mcld::Target &T) {
    TargetRegistry::RegisterTargetMachine(T, &Allocator);
  }

private:
  static mcld::LLVMTargetMachine *Allocator(const mcld::Target &T,
                                            llvm::TargetMachine& TM,
                                            const std::string &Triple) {
    return new TargetMachineImpl(TM, T, Triple);
  }
};

} //end namespace mcld

#endif

