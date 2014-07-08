//===- MCLDTargetMachine.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/CodeGen/TargetMachine.h>

#include <mcld/Module.h>
#include <mcld/LinkerConfig.h>
#include <mcld/CodeGen/MCLinker.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/ToolOutputFile.h>
#include <mcld/Target/TargetLDBackend.h>

#include <llvm/Analysis/Passes.h>
#include <llvm/CodeGen/AsmPrinter.h>
#include <llvm/CodeGen/MachineFunctionAnalysis.h>
#include <llvm/CodeGen/MachineModuleInfo.h>
#include <llvm/CodeGen/GCStrategy.h>
#include <llvm/CodeGen/Passes.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/IRPrintingPasses.h>
#include <llvm/IR/Verifier.h>
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
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/Target/TargetLowering.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Target/TargetSubtargetInfo.h>
#include <llvm/Target/TargetLoweringObjectFile.h>
#include <llvm/Target/TargetRegisterInfo.h>
#include <llvm/Transforms/Scalar.h>

#include <string>

using namespace mcld;
using namespace llvm;

//===----------------------------------------------------------------------===//
/// Arguments
//===----------------------------------------------------------------------===//
// Enable or disable FastISel. Both options are needed, because
// FastISel is enabled by default with -fast, and we wish to be
// able to enable or disable fast-isel independently from -O0.
static cl::opt<cl::boolOrDefault>
ArgEnableFastISelOption("lfast-isel", cl::Hidden,
  cl::desc("Enable the \"fast\" instruction selector"));

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

static bool getVerboseAsm(TargetMachine &TM) {
  switch (ArgAsmVerbose) {
  default:
  case cl::BOU_UNSET: return TM.getAsmVerbosityDefault();
  case cl::BOU_TRUE:  return true;
  case cl::BOU_FALSE: return false;
  }
}


//===----------------------------------------------------------------------===//
// MCLDTargetMachine
//===----------------------------------------------------------------------===//
mcld::MCLDTargetMachine::MCLDTargetMachine(llvm::TargetMachine &pTM,
                                           const llvm::Target& pLLVMTarget,
                                           const mcld::Target& pMCLDTarget,
                                           const std::string& pTriple)
  : m_TM(pTM),
    m_pLLVMTarget(&pLLVMTarget),
    m_pMCLDTarget(&pMCLDTarget),
    m_Triple(pTriple) {
}

mcld::MCLDTargetMachine::~MCLDTargetMachine()
{
  m_pLLVMTarget = NULL;
  m_pMCLDTarget = NULL;
}

const mcld::Target& mcld::MCLDTargetMachine::getTarget() const
{
  return *m_pMCLDTarget;
}

/// Turn exception handling constructs into something the code generators can
/// handle.
static void addPassesToHandleExceptions(llvm::TargetMachine *TM,
                                        llvm::legacy::PassManagerBase &PM) {
  switch (TM->getMCAsmInfo()->getExceptionHandlingType()) {
  case llvm::ExceptionHandling::SjLj:
    // SjLj piggy-backs on dwarf for this bit. The cleanups done apply to both
    // Dwarf EH prepare needs to be run after SjLj prepare. Otherwise,
    // catch info can get misplaced when a selector ends up more than one block
    // removed from the parent invoke(s). This could happen when a landing
    // pad is shared by multiple invokes and is also a target of a normal
    // edge from elsewhere.
    PM.add(createSjLjEHPreparePass(TM));
    // FALLTHROUGH
  case llvm::ExceptionHandling::DwarfCFI:
  case llvm::ExceptionHandling::ARM:
  case llvm::ExceptionHandling::WinEH:
    PM.add(createDwarfEHPass(TM));
    break;
  case llvm::ExceptionHandling::None:
    PM.add(createLowerInvokePass());

    // The lower invoke pass may create unreachable code. Remove it.
    PM.add(createUnreachableBlockEliminationPass());
    break;
  }
}


static llvm::MCContext *
addPassesToGenerateCode(llvm::LLVMTargetMachine *TM,
                        llvm::legacy::PassManagerBase &PM,
                        bool DisableVerify)
{
  // Targets may override createPassConfig to provide a target-specific sublass.
  TargetPassConfig *PassConfig = TM->createPassConfig(PM);

  // Set PassConfig options provided by TargetMachine.
  PassConfig->setDisableVerify(DisableVerify);

  PM.add(PassConfig);

  PassConfig->addIRPasses();

  addPassesToHandleExceptions(TM, PM);

  PassConfig->addISelPrepare();

  // Install a MachineModuleInfo class, which is an immutable pass that holds
  // all the per-module stuff we're generating, including MCContext.
  MachineModuleInfo *MMI =
    new MachineModuleInfo(*TM->getMCAsmInfo(), *TM->getRegisterInfo(),
                          &TM->getTargetLowering()->getObjFileLowering());
  PM.add(MMI);
  MCContext *Context = &MMI->getContext(); // Return the MCContext by-ref.

  // Set up a MachineFunction for the rest of CodeGen to work on.
  PM.add(new MachineFunctionAnalysis(*TM));

  // Enable FastISel with -fast, but allow that to be overridden.
  if (ArgEnableFastISelOption == cl::BOU_TRUE ||
      (TM->getOptLevel() == CodeGenOpt::None &&
       ArgEnableFastISelOption != cl::BOU_FALSE))
    TM->setFastISel(true);

  // Ask the target for an isel.
  if (PassConfig->addInstSelector())
    return NULL;

  PassConfig->addMachinePasses();

  PassConfig->setInitialized();

  return Context;

}

bool
mcld::MCLDTargetMachine::addPassesToEmitFile(llvm::legacy::PassManagerBase &pPM,
                                             mcld::ToolOutputFile& pOutput,
                                             mcld::CodeGenFileType pFileType,
                                             CodeGenOpt::Level pOptLvl,
                                             mcld::Module& pModule,
                                             LinkerConfig& pConfig,
                                             bool pDisableVerify)
{

  llvm::MCContext* Context =
          addPassesToGenerateCode(static_cast<llvm::LLVMTargetMachine*>(&m_TM),
                                  pPM, pDisableVerify);
  if (!Context)
    return true;

  switch(pFileType) {
  default:
  case mcld::CGFT_NULLFile:
    assert(0 && "fatal: file type is not set!");
    break;
  case CGFT_ASMFile: {
    assert(Context != 0 && "Failed to get MCContext");

    if (getTM().Options.MCOptions.MCSaveTempLabels)
      Context->setAllowTemporaryLabels(false);

    if (addCompilerPasses(pPM,
                          pOutput.formatted_os(),
                          Context))
      return true;
    break;
  }
  case CGFT_OBJFile: {
    assert(Context != 0 && "Failed to get MCContext");

    if (getTM().Options.MCOptions.MCSaveTempLabels)
      Context->setAllowTemporaryLabels(false);
    if (addAssemblerPasses(pPM,
                           pOutput.formatted_os(),
                           Context))
      return true;
    break;
  }
  case CGFT_EXEFile: {
    pConfig.setCodeGenType(LinkerConfig::Exec);
    if (addLinkerPasses(pPM,
                        pConfig,
                        pModule,
                        pOutput.fd(),
                        Context))
      return true;
    break;
  }
  case CGFT_BINARY: {
    pConfig.setCodeGenType(LinkerConfig::Binary);
    if (addLinkerPasses(pPM,
                        pConfig,
                        pModule,
                        pOutput.fd(),
                        Context))
      return true;
    break;
  }
  case CGFT_DSOFile: {
    pConfig.setCodeGenType(LinkerConfig::DynObj);
    if (addLinkerPasses(pPM,
                        pConfig,
                        pModule,
                        pOutput.fd(),
                        Context))
      return true;
    break;
  }
  case CGFT_PARTIAL: {
    pConfig.setCodeGenType(LinkerConfig::Object);
    if (addLinkerPasses(pPM,
                        pConfig,
                        pModule,
                        pOutput.fd(),
                        Context))
      return true;
    break;
  }
  } // switch
  return false;
}

bool
mcld::MCLDTargetMachine::addCompilerPasses(llvm::legacy::PassManagerBase &pPM,
                                           llvm::formatted_raw_ostream &pOutput,
                                           llvm::MCContext *&Context)
{
  const MCAsmInfo &MAI = *getTM().getMCAsmInfo();
  const MCInstrInfo &MII = *getTM().getInstrInfo();
  const MCRegisterInfo &MRI = *getTM().getRegisterInfo();
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();

  MCInstPrinter *InstPrinter =
    m_pLLVMTarget->createMCInstPrinter(MAI.getAssemblerDialect(), MAI,
                                       MII, *Context->getRegisterInfo(), STI);

  MCCodeEmitter* MCE = 0;
  MCAsmBackend *MAB = 0;
  if (ArgShowMCEncoding) {
    MCE = m_pLLVMTarget->createMCCodeEmitter(MII, MRI, STI, *Context);
    MAB = m_pLLVMTarget->createMCAsmBackend(MRI, m_Triple,
                                            getTM().getTargetCPU());
  }


  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  std::unique_ptr<MCStreamer> AsmStreamer(
    m_pLLVMTarget->createAsmStreamer(*Context, pOutput,
                                     getVerboseAsm(getTM()),
                                     getTM().Options.MCOptions.MCUseDwarfDirectory,
                                     InstPrinter,
                                     MCE, MAB,
                                     ArgShowMCInst));

  llvm::MachineFunctionPass* funcPass =
    m_pLLVMTarget->createAsmPrinter(getTM(), *AsmStreamer.get());

  if (funcPass == 0)
    return true;
  // If successful, createAsmPrinter took ownership of AsmStreamer
  AsmStreamer.release();
  pPM.add(funcPass);
  return false;
}

bool
mcld::MCLDTargetMachine::addAssemblerPasses(llvm::legacy::PassManagerBase &pPM,
                                            llvm::raw_ostream &pOutput,
                                            llvm::MCContext *&Context)
{
  // MCCodeEmitter
  const MCInstrInfo &MII = *getTM().getInstrInfo();
  const MCRegisterInfo &MRI = *getTM().getRegisterInfo();
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
  MCCodeEmitter* MCE =
    m_pLLVMTarget->createMCCodeEmitter(MII, MRI, STI, *Context);

  // MCAsmBackend
  MCAsmBackend* MAB =
    m_pLLVMTarget->createMCAsmBackend(MRI, m_Triple, getTM().getTargetCPU());
  if (MCE == 0 || MAB == 0)
    return true;

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  std::unique_ptr<MCStreamer> AsmStreamer(m_pLLVMTarget->createMCObjectStreamer(
    m_Triple, *Context, *MAB, pOutput, MCE, STI,
    getTM().Options.MCOptions.MCRelaxAll, getTM().Options.MCOptions.MCNoExecStack));

  AsmStreamer.get()->InitSections();
  MachineFunctionPass *funcPass =
    m_pLLVMTarget->createAsmPrinter(getTM(), *AsmStreamer.get());
  if (funcPass == 0)
    return true;
  // If successful, createAsmPrinter took ownership of AsmStreamer
  AsmStreamer.release();
  pPM.add(funcPass);
  return false;
}

bool
mcld::MCLDTargetMachine::addLinkerPasses(llvm::legacy::PassManagerBase &pPM,
                                         LinkerConfig& pConfig,
                                         mcld::Module& pModule,
                                         mcld::FileHandle& pFileHandle,
                                         llvm::MCContext *&Context)
{
  // set up output's SOName
  if (pConfig.options().soname().empty()) {
    // if the output is a shared object, and the option -soname was not
    // enable, set soname as the output file name. soname must be UTF-8 string.
    pConfig.options().setSOName(pFileHandle.path().filename().native());
  }

  // set up output module name
  pModule.setName(pFileHandle.path().filename().native());

  MachineFunctionPass* funcPass = m_pMCLDTarget->createMCLinker(m_Triple,
                                                                pConfig,
                                                                pModule,
                                                                pFileHandle);
  if (NULL == funcPass)
    return true;

  pPM.add(funcPass);
  return false;
}

