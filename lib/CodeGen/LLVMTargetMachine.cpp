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
#include <mcld/CodeGen/SectLinker.h>
#include <mcld/MC/MCLDDriver.h>
#include <string>

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

//===----------------------------------------------------------------------===//
/// Arguments
static cl::opt<std::string>
ArgEntry(cl::value_desc("test"),
         "e",
         cl::desc("entry_pointer")
         );

//===----------------------------------------------------------------------===//
/// LLVMTargetMachine
mcld::LLVMTargetMachine::LLVMTargetMachine(llvm::TargetMachine &pTM,
                                           const mcld::Target& pTarget,
                                           const std::string& pTriple )
  : m_TM(pTM), m_pTarget(&pTarget), m_Triple(pTriple) {
}

mcld::LLVMTargetMachine::~LLVMTargetMachine() {
  m_pTarget = 0;
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
  assert(Context != 0 && "Failed to get MCContext");

  if (getTM().hasMCSaveTempLabels())
    Context->setAllowTemporaryLabels(false);

  const MCAsmInfo &MAI = *getTM().getMCAsmInfo();
  
  llvm::MCCodeEmitter *MCE = getTarget().get()->createCodeEmitter(getTM(), *Context);
  llvm::TargetAsmBackend *TAB = getTarget().get()->createAsmBackend(m_Triple);
  if (MCE == 0 || TAB == 0)
    return true;

  OwningPtr<MCStreamer> AsmStreamer;
  AsmStreamer.reset(getTarget().get()->createObjectStreamer(m_Triple,
                                                            *Context,
                                                            *TAB, pOS, MCE,
                                                            getTM().hasMCRelaxAll(),
                                                            getTM().hasMCNoExecStack()));
  AsmStreamer.get()->InitSections();
  AsmPrinter* Printer = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer.get());
  if (Printer == 0)
    return true;

  // If successful, createAsmPrinter took ownership of AsmStreamer
  AsmStreamer.take();

  TargetLDBackend *TDB = getTarget().createLDBackend(*getTarget().get(), m_Triple);
  if (TDB == 0)
    return true;
  // SectLinker has the ownership of TargetLDBackend
  FunctionPass* Linker = getTarget().createSectLinker(m_Triple, *Printer, *TDB);

  switch( pFileType ) {
  default: return true;
  case CGFT_DSOFile: {
    break;
  }
  case CGFT_EXEFile: {
    break;
  }
  case mcld::CGFT_Null: 
    assert("Null File");
    break;
  }

  pPM.add(Linker);

  setCodeModelForStatic();
  pPM.add(createGCInfoDeleter()); // not in addPassesToMC
  return false;
}

