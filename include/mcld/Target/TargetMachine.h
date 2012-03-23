//===- TargetMachine.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_MACHINE_H
#define MCLD_TARGET_MACHINE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Target/TargetMachine.h>
#include <string>
#include "mcld/MC/MCLDFile.h"

namespace llvm
{
class Target;
class TargetData;
class TargetMachine;
class PassManagerBase;
class formatted_raw_ostream;

} // namespace of llvm

namespace mcld
{

class Target;
class MCLDInfo;
class SectLinkerOption;
using namespace llvm;

enum CodeGenFileType {
  CGFT_ASMFile,
  CGFT_OBJFile,
  CGFT_DSOFile,
  CGFT_EXEFile,
  CGFT_NULLFile
};


/** \class mcld::LLVMTargetMachine
 *  \brief mcld::LLVMTargetMachine is a object adapter of
 *  llvm::LLVMTargetMachine.
 *
 *  mcld::LLVMTargetMachine is also in charge of MCLDInfo.
 *
 *  @see MCLDInfo
 */
class LLVMTargetMachine
{
public:
  /// Adapter of llvm::TargetMachine
  ///
  LLVMTargetMachine(llvm::TargetMachine &pTM,
                    const mcld::Target &pTarget,
                    const std::string &pTriple);
  virtual ~LLVMTargetMachine();

  /// getTarget - adapt llvm::TargetMachine::getTarget
  const mcld::Target& getTarget() const;

  /// getTM - return adapted the llvm::TargetMachine.
  const llvm::TargetMachine& getTM() const { return m_TM; }
  llvm::TargetMachine& getTM() { return m_TM; }

  /// getLDInfo - return the mcld::MCLDInfo
  virtual mcld::MCLDInfo& getLDInfo() = 0;
  virtual const mcld::MCLDInfo& getLDInfo() const = 0;

  /// appPassesToEmitFile - The target function which we has to modify as
  /// upstreaming.
  bool addPassesToEmitFile(PassManagerBase &,
                           formatted_raw_ostream &Out,
                           const std::string &pOutputFilename,
                           mcld::CodeGenFileType,
                           CodeGenOpt::Level,
                           SectLinkerOption *pLinkerOpt = NULL,
                           bool DisableVerify = true);

  /// getTargetData
  const TargetData *getTargetData() const { return m_TM.getTargetData(); }

  /// setAsmVerbosityDefault
  static void setAsmVerbosityDefault(bool pAsmVerbose) {
    llvm::TargetMachine::setAsmVerbosityDefault(pAsmVerbose);
  }

private:
  /// addCommonCodeGenPasses - Add standard LLVM codegen passes used for
  /// both emitting to assembly files or machine code output.
  bool addCommonCodeGenPasses(PassManagerBase &,
                              mcld::CodeGenFileType,
                              CodeGenOpt::Level,
                              bool DisableVerify,
                              llvm::MCContext *&OutCtx);

  bool addCompilerPasses(PassManagerBase &,
                         formatted_raw_ostream &Out,
                         const std::string& pOutputFilename,
                         llvm::MCContext *&OutCtx);

  bool addAssemblerPasses(PassManagerBase &,
                          formatted_raw_ostream &Out,
                          const std::string& pOutputFilename,
                          llvm::MCContext *&OutCtx);

  bool addLinkerPasses(PassManagerBase &,
                       SectLinkerOption *pLinkerOpt,
                       const std::string& pOutputFilename,
                       MCLDFile::Type pOutputLinkType,
                       llvm::MCContext *&OutCtx);

private:
  llvm::TargetMachine &m_TM;
  const mcld::Target *m_pTarget;
  const std::string& m_Triple;
};

} // namespace of mcld

#endif

