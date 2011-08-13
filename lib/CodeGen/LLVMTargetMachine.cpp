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
#include <mcld/MC/MCAsmObjectReader.h>
#include <mcld/MC/MCLDFile.h>
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
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/MC/MCInstrInfo.h>
#include <llvm/MC/MCSubtargetInfo.h>
#include <llvm/Target/TargetSubtargetInfo.h>
#include <llvm/Target/TargetInstrInfo.h>
#include <llvm/MC/MCObjectStreamer.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCObjectWriter.h>


using namespace mcld;
using namespace llvm;

//===----------------------------------------------------------------------===//
/// Arguments
static cl::opt<bool>
ArgShowMCEncoding("lshow-mc-encoding",
                cl::Hidden,
                cl::desc("Show encoding in .s output"));

static cl::opt<bool>
ArgAsmVerbose("fverbose-asm",
           cl::desc("Put extra commentary information in the \
generated assembly code to make it more readable."));

static cl::opt<bool>
ArgShowMCInst("lshow-mc-inst",
              cl::Hidden,
              cl::desc("Show instruction structure in .s output"));


//===---------------------------------------------------------------------===//
/// Non-member functions
static tool_output_file *GetOutputStream(const std::string& pOutputFilename)
{
  std::string error;
  unsigned OpenFlags = 0;
  if (pFileType != mcld::CGFT_ASMFile)
    OpenFlags |= raw_fd_ostream::F_Binary;
  tool_output_file *FDOut = new tool_output_file(pOutputFilename.c_str(),
                                                 error,
                                                 OpenFlags);
  if (!error.empty()) {
    errs() << error << '\n';
    delete FDOut;
    return 0;
  }

  return FDOut;
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
                                                     MCContext *&OutCtx)
{
  return static_cast<llvm::LLVMTargetMachine&>(m_TM).addCommonCodeGenPasses(
                                            PM, Level, DisableVerify, OutCtx);
}

bool mcld::LLVMTargetMachine::addPassesToEmitFile(
                                             PassManagerBase &pPM,
                                             const std::string& pInputFilename,
                                             const std::string& pOutputFilename,
                                             mcld::CodeGenFileType pFileType,
                                             CodeGenOpt::Level pOptLvl,
                                             bool pDisableVerify)
{

  // MCContext
  MCContext* Context = 0;
  if (addCommonCodeGenPasses(pPM, pOptLvl, pDisableVerify, Context))
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
                          pOutputFilename,
                          Context))
      return true;
    break;
  }
  case CGFT_OBJFile: {
    if (addAssemblerPasses(pPM,
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
    if (addLinkerPasses(pPM,
                        pInputFilename,
                        pOutputFilename,
                        MCLDFile::Exec,
                        Context))
      return true;
    break;
  }
  case CGFT_DSOFile: {
    if (addLinkerPasses(pPM,
                        pInputFilename,
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
                                                const std::string& pOutputFilename,
                                                MCContext *&Context)
{
  const MCAsmInfo &MAI = *getTM().getMCAsmInfo();

  MCInstPrinter *InstPrinter =
    getTarget().get()->createMCInstPrinter(MAI.getAssemblerDialect(), MAI);

  MCCodeEmitter* MCE = 0;
  // MCCodeEmitter
  if (ArgShowMCEncoding) {
#if LLVM_VERSION > 2
    const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
    MCE = getTarget().get()->createMCCodeEmitter(*(getTM().getInstrInfo()), STI, *Context);
#else
    MCE = getTarget().get()->createMCCodeEmitter(getTM(), *Context);
#endif
  }

  // MCAsmBackend
  MCAsmBackend *MAB = getTarget().get()->createMCAsmBackend(m_Triple);
  if (MCE == 0 || MAB == 0)
    return true;

  tool_output_file *Output = GetOutputStream(pOutputFilename);

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  OwningPtr<MCStreamer> AsmStreamer(getTarget().get()->createAsmStreamer(*Context,
                                                                         Output->os(),
                                                                         ArgAsmVerbose,
                                                                         getTM().hasMCUseLoc(),
                                                                         getTM().hasMCUseCFI(),
                                                                         InstPrinter,
                                                                         MCE,
                                                                         MAB,
                                                                         ArgShowMCInst));

  llvm::MachineFunctrionPass* funcPass = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer.get());
  if (funcPass == 0)
    return true;
  // If successful, createAsmPrinter took ownership of AsmStreamer
  AsmStreamer.take();
  pPM.add(funcPass);
  return false;
}

bool mcld::LLVMTargetMachine::addAssemblerPasses(PassManagerBase &pPM,
                                                 const std::string& pOutputFilename,
                                                 MCContext *&Context)
{
  // MCCodeEmitter
#if LLVM_VERSION > 2
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
  MCCodeEmitter* MCE = getTarget().get()->createMCCodeEmitter(*getTM().getInstrInfo(), STI, *Context);
#else
  MCCodeEmitter* MCE = getTarget().get()->createMCCodeEmitter(getTM(), *Context);
#endif

  // MCAsmBackend
  MCAsmBackend* MAB = getTarget().get()->createMCAsmBackend(m_Triple);
  if (MCE == 0 || MAB == 0)
    return true;

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  tool_output_file *Output = GetOutputStream(pOutputFilename);
  OwningPtr<MCStreamer> AsmStreamer(getTarget().get()->createMCObjectStreamer(
                                                              m_Triple,
                                                              *Context,
                                                              *MAB,
                                                              Output->os(),
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
                                              const std::string& pInputFilename,
                                              const std::string& pOutputFilename,
                                              unsigned int pOutputLinkType,
                                              MCContext *&Context)
{
  // Initialize MCAsmStreamer first, than chain its output into SectLinker.
  // MCCodeEmitter
#if LLVM_VERSION > 2
  const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
  MCCodeEmitter* MCE = getTarget().get()->createMCCodeEmitter(*getTM().getInstrInfo(),
                                                              STI,
                                                              *Context);
#else
  MCCodeEmitter* MCE = getTarget().get()->createMCCodeEmitter(getTM(),
                                                              *Context);
#endif
  // MCAsmBackend
  MCAsmBackend *MAB = getTarget().get()->createMCAsmBackend(m_Triple);
  if (MCE == 0 || MAB == 0)
    return true;

  // now, we have MCCodeEmitter and MCAsmBackend, we can create AsmStreamer.
  OwningPtr<MCStreamer> AsmStreamer(
                            getTarget().get()->createMCObjectStreamer(
                                                  m_Triple,
                                                  *Context,
                                                  *MAB,
                                                  llvm::nulls(),
                                                  MCE,
                                                  getTM().hasMCRelaxAll(),
                                                  getTM().hasMCNoExecStack()));

  MCAsmObjectReader *objReader = 
                         new MCAsmObjectReader(
                                *static_cast<MCObjectStreamer*>(AsmStreamer),
                                getLDInfo());

  AsmStreamer->InitSections();
  AsmPrinter* printer = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer);
  if (0 == printer )
    return true;
  pPM.add(printer);

  TargetLDBackend* ldBackend = getTarget().createLDBackend(*getTarget().get(), m_Triple);
  if (0 == ldBackend)
    return true;

  MachineFunctionPass* funcPass = getTarget().createSectLinker(m_Triple, 
                                                               pInputFilename,
                                                               pOutputFilename,
                                                               pLinkType,
                                                               getLDInfo(),
                                                               *ldBackend);
  if (0 == funcPass)
    return true;
  pPM.add(funcPass);
  return false;
}

