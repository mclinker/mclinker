/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#include <mcld/Target/TargetMachine.h>

#include "llvm/PassManager.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/PrintModulePass.h"
#include "llvm/CodeGen/AsmPrinter.h"
#include "llvm/CodeGen/MachineFunctionAnalysis.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/CodeGen/GCStrategy.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/Target/TargetAsmInfo.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ADT/OwningPtr.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/FormattedStream.h"



using namespace mcld;
using namespace llvm;

namespace mcld
{

//==========================
// TargetMachine
bool addPassesToEmitFile(TargetMachine &pTarget,
			   PassManagerBase &pPM,
			   formatted_raw_ostream &pOS,
			   mcld::CodeGenFileType pFileType,
			   CodeGenOpt::Level pOptLvl,
			   bool pDisableVerify ) 
{

  MCContext* Context = 0;
  if (pTarget.addCommonCodeGenPasses( pPM, pOptLvl, pDisableVerify, Context))
    return true;

  //not in addPassesToMC
  if (hasMCSaveTempLabels())
    Context->setAllowTemporaryLabels(false);

  OwningPtr<MCStreamer> AsmStreamer;


  switch( pFileType ) {
  default: return true;
  case CGFT_DSOFile: {
    MCCodeEmitter *MCE = getTarget().createCodeEmitter(*this, *Context);
    TargetAsmBackend *TAB = getTarget().createAsmBackend(TargetTriple);

    // FIXME: TargetLDBackend needs implementation
    TargetLDBackend *TDB = getTarget().createLDBackend(TargetTriple);   

    if (MCE == 0 || TAB == 0)
      return true;

    AsmStreamer.reset(getTarget().createObjectStreamer(TargetTriple, *Context,
                                                       *TAB, Out, MCE,
                                                       hasMCRelaxAll(),
                                                       hasMCNoExecStack()));

    AsmStreamer.get()->InitSections();
   
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

  // not in addPassesToMC
  if (EnableMCLogging)
    AsmStreamer.reset(createLoggingStreamer(AsmStreamer.take(), errs()));

  // Create the AsmPrinter, which takes ownership of AsmStreamer if successful.
  FunctionPass *Printer = getTarget().createAsmPrinter(*this, *AsmStreamer);
  if (Printer == 0)
    return true;

  // If successful, createAsmPrinter took ownership of AsmStreamer.
  AsmStreamer.take();

  PM.add(Printer);

  // Make sure the code model is set.
  setCodeModelForStatic(); //using setCodeModelForJIT() in addPassesToMC
  PM.add(createGCInfoDeleter()); // not in addPassesToMC
  return false;
}

} // namespace of mcld
