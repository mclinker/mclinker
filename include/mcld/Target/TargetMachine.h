/*****************************************************************************
 *   The mcld Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#ifndef TARGETMACHINE_H
#define TARGETMACHINE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Target/TargetMachine.h>
#include <string>

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
using namespace llvm;

enum CodeGenFileType {
  CGFT_ASMFile,
  CGFT_OBJFile,
  CGFT_ARCFile,
  CGFT_DSOFile,
  CGFT_EXEFile,
  CGFT_NULLFile
};


/** \class mcld::LLVMTargetMachine
 *  \brief mcld::LLVMTargetMachine is a object adapter of 
 *  llvm::LLVMTargetMachine.
 *
 *  @author Luba Tang <lubatang@mediatek.com>
 */
class LLVMTargetMachine
{
public:
  /// Adapter of llvm::TargetMachine
  ///
  LLVMTargetMachine(llvm::TargetMachine &pTM,
                    const mcld::Target &pTarget,
                    const std::string &pTriple);
  ~LLVMTargetMachine();

  /// getTarget - adapt llvm::TargetMachine::getTarget
  ///
  const mcld::Target& getTarget() const;

  /// getTargetMachine - return adapted the llvm::TargetMachine.
  ///
  const llvm::TargetMachine& getTM() const { return m_TM; }
  llvm::TargetMachine& getTM() { return m_TM; }

  /// appPassesToEmitFile - The target function which we has to modify as
  /// upstreaming.
  ///
  bool addPassesToEmitFile(PassManagerBase &,
                           formatted_raw_ostream &,
                           mcld::CodeGenFileType,
                           CodeGenOpt::Level,
                           bool DisableVerify= true); 

  /// getTargetData
  ///
  const TargetData *getTargetData() const { return m_TM.getTargetData(); }

  /// setAsmVerbosityDefault
  ///
  static void setAsmVerbosityDefault(bool pAsmVerbose) { 
    llvm::TargetMachine::setAsmVerbosityDefault(pAsmVerbose); 
  }

private:
  /// setCodeModelForStatic
  ///
  void setCodeModelForStatic();

private:
  llvm::TargetMachine &m_TM;
  const mcld::Target *m_pTarget;
  const std::string& m_Triple;
};

} // namespace of mcld

#endif

