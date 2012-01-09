//===- LLVMTargetMachine.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/CodeGen/SectLinker.h"
#include "mcld/CodeGen/SectLinkerOption.h"
#include "mcld/MC/MCBitcodeInterceptor.h"
#include "mcld/MC/MCLDFile.h"
#include "mcld/Support/RealPath.h"
#include "mcld/Support/TargetRegistry.h"
#include "mcld/Target/TargetMachine.h"
#include "mcld/Target/TargetLDBackend.h"

#include <llvm/ADT/OwningPtr.h>
#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/MachineFunctionAnalysis.h>
#include <llvm/CodeGen/MachineModuleInfo.h>
#include <llvm/CodeGen/GCStrategy.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/MC/MCAsmInfo.h>
#include <llvm/MC/MCStreamer.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/MC/MCObjectStreamer.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCObjectWriter.h>
#include <llvm/MC/MCContext.h>
#include <llvm/PassManager.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetSubtargetInfo.h>
#include <llvm/Transforms/Scalar.h>

#include <string>

using namespace mcld;
using namespace llvm;

//===----------------------------------------------------------------------===//
/// Arguments
static cl::opt<bool>
ArgShowMCEncoding("lshow-mc-encoding",
                cl::Hidden,
                cl::desc("Show encoding in .s output"));

static cl::opt<bool>
ArgShowMCInst("lshow-mc-inst",
              cl::Hidden,
              cl::desc("Show instruction structure in .s output"));

static cl::opt<cl::boolOrDefault>
ArgAsmVerbose("fverbose-asm",
              cl::desc("Put extra commentary information in the \
                       generated assembly code to make it more readable."),
              cl::init(cl::BOU_UNSET));

static bool getVerboseAsm() {
  switch (ArgAsmVerbose) {
  default:
  case cl::BOU_UNSET: return TargetMachine::getAsmVerbosityDefault();
  case cl::BOU_TRUE:  return true;
  case cl::BOU_FALSE: return false;
  }
}


//===---------------------------------------------------------------------===//
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

bool mcld::LLVMTargetMachine::addCommonCodeGenPasses(PassManagerBase &PM,
                                                     CodeGenOpt::Level Level,
                                                     bool DisableVerify,
                                                     llvm::MCContext *&OutCtx)
{
  return static_cast<llvm::LLVMTargetMachine&>(m_TM).addCommonCodeGenPasses(
                                            PM, DisableVerify, OutCtx);
}

bool mcld::LLVMTargetMachine::addPassesToEmitFile(PassManagerBase &pPM,
                                             formatted_raw_ostream &Out,
                                             const std::string& pOutputFilename,
                                             mcld::CodeGenFileType pFileType,
                                             CodeGenOpt::Level pOptLvl,
                                             SectLinkerOption *pLinkerOpt,
                                             bool pDisableVerify)
{

  // MCContext
  llvm::MCContext* Context = 0;
  if (addCommonCodeGenPasses(pPM, pOptLvl,pDisableVerify, Context))
    return true;
  assert(Context != 0 && "Failed to get MCContext");

  if (getTM().hasMCSaveTempLabels())
    Context->setAllowTemporaryLabels(false);

  switch( pFileType ) {
  default:
  case mcld::CGFT_NULLFile:
    assert(0 && "fatal: file type is not set!");
    break;
  case CGFT_ASMFile: {
    if (addCompilerPasses(pPM,
                          Out,
                          pOutputFilename,
                          Context))
      return true;
    break;
  }
  case CGFT_OBJFile: {
    if (addAssemblerPasses(pPM,
                           Out,
                           pOutputFilename,
                           Context))
      return true;
    break;
  }
  case CGFT_ARCFile: {
    assert(0 && "Output to archive file has not been supported yet!");
    break;
  }
  case CGFT_EXEFile: {
    if (pLinkerOpt == NULL)
      return true;

    if (addLinkerPasses(pPM,
                        pLinkerOpt,
                        pOutputFilename,
                        MCLDFile::Exec,
                        Context))
      return true;
    break;
  }
  case CGFT_DSOFile: {
    if (pLinkerOpt == NULL)
      return true;

    if (addLinkerPasses(pPM,
                        pLinkerOpt,
                        pOutputFilename,
                        MCLDFile::DynObj,
                        Context))
      return true;
    break;
  }
  } // switch

  pPM.add(createGCInfoDeleter()); // not in addPassesToMC
  return false;
}

bool mcld::LLVMTargetMachine::addCompilerPasses(PassManagerBase &pPM,
                                                formatted_raw_ostream &Out,
                                                const std::string& pOutputFilename,
                                                llvm::MCContext *&Context)
{
  const MCAsmInfo &MAI = *getTM().getMCAsmInfo();
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();

  MCInstPrinter *InstPrinter =
    getTarget().get()->createMCInstPrinter(MAI.getAssemblerDialect(), MAI, STI);

  MCCodeEmitter* MCE = 0;
  // MCCodeEmitter
  if (ArgShowMCEncoding) {
    MCE = getTarget().get()->createMCCodeEmitter(*(getTM().getInstrInfo()), STI, *Context);
  }

  // MCAsmBackend
  MCAsmBackend *MAB = getTarget().get()->createMCAsmBackend(m_Triple);
  if (MCE == 0 || MAB == 0)
    return true;

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  OwningPtr<MCStreamer> AsmStreamer(
    getTarget().get()->createAsmStreamer(*Context, Out,
                                         getVerboseAsm(),
                                         getTM().hasMCUseLoc(),
                                         getTM().hasMCUseCFI(),
                                         getTM().hasMCUseDwarfDirectory(),
                                         InstPrinter,
                                         MCE, MAB,
                                         ArgShowMCInst));

  llvm::MachineFunctionPass* funcPass =
    getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer.get());

  if (funcPass == 0)
    return true;
  // If successful, createAsmPrinter took ownership of AsmStreamer
  AsmStreamer.take();
  pPM.add(funcPass);
  return false;
}

bool mcld::LLVMTargetMachine::addAssemblerPasses(PassManagerBase &pPM,
                                                 formatted_raw_ostream &Out,
                                                 const std::string& pOutputFilename,
                                                 llvm::MCContext *&Context)
{
  // MCCodeEmitter
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
  MCCodeEmitter* MCE = getTarget().get()->createMCCodeEmitter(*getTM().getInstrInfo(), STI, *Context);

  // MCAsmBackend
  MCAsmBackend* MAB = getTarget().get()->createMCAsmBackend(m_Triple);
  if (MCE == 0 || MAB == 0)
    return true;

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  OwningPtr<MCStreamer> AsmStreamer(getTarget().get()->createMCObjectStreamer(
                                                              m_Triple,
                                                              *Context,
                                                              *MAB,
                                                              Out,
                                                              MCE,
                                                              getTM().hasMCRelaxAll(),
                                                              getTM().hasMCNoExecStack()));
  AsmStreamer.get()->InitSections();
  MachineFunctionPass *funcPass = getTarget().get()->createAsmPrinter(getTM(),
                                                                      *AsmStreamer.get());
  if (funcPass == 0)
    return true;
  // If successful, createAsmPrinter took ownership of AsmStreamer
  AsmStreamer.take();
  pPM.add(funcPass);
  return false;
}

bool mcld::LLVMTargetMachine::addLinkerPasses(PassManagerBase &pPM,
                                              SectLinkerOption *pLinkerOpt,
                                              const std::string &pOutputFilename,
                                              MCLDFile::Type pOutputLinkType,
                                              llvm::MCContext *&Context)
{
  // Initialize MCAsmStreamer first, than chain its output into SectLinker.
  // MCCodeEmitter
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
  MCCodeEmitter* MCE = getTarget().get()->createMCCodeEmitter(*getTM().getInstrInfo(),
                                                              STI,
                                                              *Context);
  // MCAsmBackend
  MCAsmBackend *MAB = getTarget().get()->createMCAsmBackend(m_Triple);
  if (MCE == 0 || MAB == 0)
    return true;

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  MCStreamer* AsmStreamer =
    getTarget().get()->createMCObjectStreamer(m_Triple,
                                              *Context,
                                              *MAB,
                                              llvm::nulls(),
                                              MCE,
                                              getTM().hasMCRelaxAll(),
                                              getTM().hasMCNoExecStack());
  if (0 == AsmStreamer)
    return true;

  AsmStreamer->InitSections();
  AsmPrinter* printer = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer);
  if (0 == printer)
    return true;
  pPM.add(printer);

  TargetLDBackend* ldBackend = getTarget().createLDBackend(*getTarget().get(), m_Triple);
  if (0 == ldBackend)
    return true;

  MCBitcodeInterceptor* objReader = new MCBitcodeInterceptor(
                                 static_cast<MCObjectStreamer&>(*AsmStreamer),
                                 *ldBackend,
                                 getLDInfo());

  pLinkerOpt->info().output().setPath(sys::fs::RealPath(pOutputFilename));
  pLinkerOpt->info().output().setType(pOutputLinkType);

  MachineFunctionPass* funcPass = getTarget().createSectLinker(m_Triple,
                                                               *pLinkerOpt,
                                                               *ldBackend);
  if (0 == funcPass)
    return true;
  pPM.add(funcPass);
  return false;
}

