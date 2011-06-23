/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/Target/TargetRegistry.h>
#include <mcld/Target/TargetMachine.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDDriver.h>

#include <llvm/PassManager.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/MachineFunctionAnalysis.h>
#include <llvm/CodeGen/MachineModuleInfo.h>
#include <llvm/CodeGen/GCStrategy.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCStreamer.h>
#include <llvm/Target/TargetAsmInfo.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/FormattedStream.h>



using namespace mcld;
using namespace llvm;

/* ** */

mcld::LLVMTargetMachine::LLVMTargetMachine(llvm::TargetMachine &pTM, const std::string& pTriple)
  : m_TM(pTM), m_Triple(pTriple) {
   m_pTarget = new mcld::Target(getTM().getTarget());
}

mcld::LLVMTargetMachine::~LLVMTargetMachine() {
  delete m_pTarget;
}

const mcld::Target& mcld::LLVMTargetMachine::getTarget() const
{
  return *m_pTarget;
}

void mcld::LLVMTargetMachine::setCodeModelForStatic()
{
  static_cast<llvm::LLVMTargetMachine&>(m_TM).setCodeModelForStatic();
}

bool mcld::LLVMTargetMachine::addPassesToEmitFile(PassManagerBase &pPM,
                                         formatted_raw_ostream &pOS,
                                         mcld::CodeGenFileType pFileType,
                                         CodeGenOpt::Level pOptLvl,
                                         bool pDisableVerify ) {

  MCContext* Context = 0;
  if (getTM().addCommonCodeGenPasses( pPM, pOptLvl, pDisableVerify, Context))
    return true;

  //not in addPassesToMC
  if (getTM().hasMCSaveTempLabels())
    Context->setAllowTemporaryLabels(false);

  OwningPtr<MCLDDriver> LDDriver;


  switch( pFileType ) {
  default: return true;
  case CGFT_DSOFile: {
    llvm::MCCodeEmitter *MCE = getTarget().createCodeEmitter(*this, *Context);
    //TargetAsmBackend *TAB = getTarget().createAsmBackend(TargetTriple);

    // FIXME: TargetLDBackend needs implementation
    TargetLDBackend *TDB = getTarget().createLDBackend();

    if (MCE == 0 || TDB == 0)
      return true;

    //AsmStreamer.reset(getTarget().createObjectStreamer(TargetTriple, *Context,
    //                                                   *TAB, Out, MCE,
    //                                                   hasMCRelaxAll(),
    //                                                   hasMCNoExecStack()));


    LDDriver.reset(getTarget().createLDDriver());

    //AsmStreamer.get()->InitSections();
   
    // create MCLDPrinter
    // PM.add();
    break;
  }
  case CGFT_EXEFile: {
    break;
  }
  case mcld::CGFT_Null: 
    assert("Null File");
    break;
  }

  // Create the AsmPrinter, which takes ownership of AsmStreamer if successful.
  // FunctionPass *Printer = getTarget().createAsmPrinter(getTM(), *AsmStreamer);
  //if (Printer == 0)
  //  return true;

  //pPM.add(Printer);

  // Make sure the code model is set.
  setCodeModelForStatic();
  pPM.add(createGCInfoDeleter()); // not in addPassesToMC
  return false;
}

