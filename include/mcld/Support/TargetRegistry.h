//===- TargetRegistry.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_TARGETREGISTRY_H
#define MCLD_SUPPORT_TARGETREGISTRY_H
#include <mcld/Support/Target.h>
#include <llvm/ADT/Triple.h>

#include <string>
#include <list>

namespace llvm {
class TargetMachine;
class MCCodeEmitter;
class MCContext;
class AsmPrinter;
} // namespace of llvm

namespace mcld {

/** \class TargetRegistry
 *  \brief TargetRegistry is an object adapter of llvm::TargetRegistry
 */
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
  static void RegisterTarget(Target& pTarget,
                             const char* pName,
                             Target::TripleMatchQualityFnTy pQualityFn);

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

  /// RegisterMCLinker - Register a MCLinker implementation for the given
  /// target.
  ///
  /// @param T - the target being registered
  /// @param Fn - A function to create MCLinker for the target
  static void RegisterMCLinker(mcld::Target &T, mcld::Target::MCLinkerCtorTy Fn)
  {
    if (!T.MCLinkerCtorFn)
      T.MCLinkerCtorFn = Fn;
  }

  /// RegisterEmulation - Register a emulation function for the target.
  /// target.
  ///
  /// @param T - the target being registered
  /// @param Fn - A emulation function
  static void RegisterEmulation(mcld::Target &T, mcld::Target::EmulationFnTy Fn)
  {
    if (!T.EmulationFn)
      T.EmulationFn = Fn;
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

  /// lookupTarget - Look up MCLinker target
  ///
  /// @param Triple - The Triple string
  /// @param Error  - The returned error message
  static const mcld::Target *lookupTarget(const std::string& pTriple,
                                          std::string& pError);

  /// lookupTarget - Look up MCLinker target by an architecture name
  /// and a triple. If the architecture name is not empty, then the
  /// the lookup is done mainly by architecture. Otherwise, the target
  /// triple is used.
  ///
  /// @param pArch   - The architecture name
  /// @param pTriple - The target triple
  /// @param pError  - The returned error message
  static const mcld::Target *lookupTarget(const std::string& pArchName,
                                          llvm::Triple& pTriple,
                                          std::string &Error);
};

/// RegisterTarget - Helper function for registering a target, for use in the
/// target's initialization function. Usage:
///
/// Target TheFooTarget; // The global target instance.
///
/// extern "C" void MCLDInitializeFooTargetInfo() {
///   RegisterTarget<llvm::Foo> X(TheFooTarget, "foo", "Foo description");
/// }
template<llvm::Triple::ArchType TargetArchType = llvm::Triple::UnknownArch>
struct RegisterTarget
{
public:
  RegisterTarget(mcld::Target &pTarget, const char* pName) {
    // if we've registered one, then return immediately.
    TargetRegistry::iterator target, ie = TargetRegistry::end();
    for (target = TargetRegistry::begin(); target != ie; ++target) {
      if (0 == strcmp((*target)->name(), pName))
        return;
    }

    TargetRegistry::RegisterTarget(pTarget, pName, &getTripleMatchQuality);
  }

  static unsigned int getTripleMatchQuality(const llvm::Triple& pTriple) {
    if (pTriple.getArch() == TargetArchType)
      return 20;
    return 0;
  }
};

/// RegisterTargetMachine - Helper template for registering a target machine
/// implementation, for use in the target machine initialization
/// function. Usage:
///
/// extern "C" void MCLDInitializeFooTarget() {
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
  static MCLDTargetMachine *Allocator(const llvm::Target& pLLVMTarget,
                                      const mcld::Target& pMCLDTarget,
                                      llvm::TargetMachine& pTM,
                                      const std::string& pTriple) {
    return new TargetMachineImpl(pTM, pLLVMTarget, pMCLDTarget, pTriple);
  }
};

} //end namespace mcld

#endif

