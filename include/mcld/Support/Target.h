//===- Target.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SUPPORT_TARGET_H
#define MCLD_SUPPORT_TARGET_H
#include <llvm/Support/TargetRegistry.h>
#include <string>
#include <list>

namespace mcld {

class MCLDTargetMachine;
class TargetRegistry;
class MCLinker;
class LinkerScript;
class LinkerConfig;
class Module;
class MemoryArea;
class DiagnosticLineInfo;
class TargetLDBackend;

//===----------------------------------------------------------------------===//
/// Target - mcld::Target is an object adapter of llvm::Target
//===----------------------------------------------------------------------===//
class Target
{
  friend class mcld::MCLDTargetMachine;
  friend class mcld::TargetRegistry;
public:
  typedef mcld::MCLDTargetMachine *(*TargetMachineCtorTy)(const mcld::Target &,
                                                          llvm::TargetMachine &,
                                                          const std::string&);

  typedef MCLinker *(*MCLinkerCtorTy)(const std::string& pTriple,
                                      LinkerConfig&,
                                      Module&,
                                      MemoryArea& pOutput);

  typedef bool (*EmulationFnTy)(LinkerScript&, LinkerConfig&);

  typedef TargetLDBackend  *(*TargetLDBackendCtorTy)(const llvm::Target&,
                                                     const LinkerConfig&);

  typedef DiagnosticLineInfo *(*DiagnosticLineInfoCtorTy)(const mcld::Target&,
                                                          const std::string&);

public:
  Target();

  void setTarget(const llvm::Target& pTarget)
  { m_pT = &pTarget; }

  MCLDTargetMachine* createTargetMachine(const std::string &pTriple,
                          const std::string &pCPU, const std::string &pFeatures,
                          const llvm::TargetOptions &Options,
                          llvm::Reloc::Model RM = llvm::Reloc::Default,
                          llvm::CodeModel::Model CM = llvm::CodeModel::Default,
                          llvm::CodeGenOpt::Level OL = llvm::CodeGenOpt::Default) const;

  /// createMCLinker - create target-specific MCLinker
  ///
  /// @return created MCLinker
  MCLinker *createMCLinker(const std::string &pTriple,
                           LinkerConfig& pConfig,
                           Module& pModule,
                           MemoryArea& pOutput) const {
    if (!MCLinkerCtorFn)
      return NULL;
    return MCLinkerCtorFn(pTriple, pConfig, pModule, pOutput);
  }

  /// emulate - given MCLinker default values for the other aspects of the
  /// target system.
  bool emulate(LinkerScript& pScript, LinkerConfig& pConfig) const {
    if (!EmulationFn)
      return false;
    return EmulationFn(pScript, pConfig);
  }

  /// createLDBackend - create target-specific LDBackend
  ///
  /// @return created TargetLDBackend
  TargetLDBackend* createLDBackend(const LinkerConfig& pConfig) const
  {
    if (!TargetLDBackendCtorFn)
      return NULL;
    return TargetLDBackendCtorFn(*get(), pConfig);
  }

  /// createDiagnosticLineInfo - create target-specific DiagnosticLineInfo
  DiagnosticLineInfo* createDiagnosticLineInfo(const mcld::Target& pTarget,
                                               const std::string& pTriple) const
  {
    if (!DiagnosticLineInfoCtorFn)
      return NULL;
    return DiagnosticLineInfoCtorFn(pTarget, pTriple);
  }

  const llvm::Target* get() const { return m_pT; }

private:
  // -----  function pointers  ----- //
  TargetMachineCtorTy TargetMachineCtorFn;
  MCLinkerCtorTy MCLinkerCtorFn;
  EmulationFnTy EmulationFn;
  TargetLDBackendCtorTy TargetLDBackendCtorFn;
  DiagnosticLineInfoCtorTy DiagnosticLineInfoCtorFn;

  // -----  adapted llvm::Target  ----- //
  const llvm::Target* m_pT;
};

} //end namespace mcld

#endif

