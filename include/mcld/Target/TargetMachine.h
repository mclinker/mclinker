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

namespace llvm
{
class Target;
class TargetData;
class PassManagerBase;
class formatted_raw_ostream;

} // namespace of llvm

namespace mcld
{

  using namespace llvm;

  enum CodeGenFileType {
    CGFT_Null = 2,
    CGFT_DSOFile,
    CGFT_EXEFile
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
    LLVMTargetMachine(llvm::TargetMachine &pTM);
    ~LLVMTargetMachine();

    /// getTarget - adapt llvm::TargetMachine::getTarget
    ///
    const llvm::Target& getTarget() const { return m_TM.getTarget(); }

    /// getTargetMachine - return adapted the llvm::TargetMachine.
    ///
    const TargetMachine& getTM() const { return m_TM; }
    TargetMachine& getTM() { return m_TM; }

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
      TargetMachine::setAsmVerbosityDefault(pAsmVerbose); 
    }

    
  private:
    llvm::TargetMachine& m_TM;
  };

} // namespace of mcld

#endif

