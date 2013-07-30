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
#include <mcld/Support/Target.h>
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
/// extern "C" void MCLDInitializeFooTargetInfo() {
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

    if (TIter != TEnd)
      T.setTarget(*TIter);

    TargetRegistry::RegisterTarget(T);
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
  static mcld::MCLDTargetMachine *Allocator(const mcld::Target &T,
                                            llvm::TargetMachine& TM,
                                            const std::string &Triple) {
    return new TargetMachineImpl(TM, T, Triple);
  }
};

} //end namespace mcld

#endif

