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
#include <llvm/Target/TargetAsmInfo.h>
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

bool mcld::LLVMTargetMachine::addCommonCodeGenPasses(PassManagerBase &PM,
                                                     CodeGenOpt::Level Level,
                                                     bool DisableVerify,
                                                     MCContext *&OutCtx)
{
  return static_cast<llvm::LLVMTargetMachine&>(m_TM).addCommonCodeGenPasses(
                                            PM, Level, DisableVerify, OutCtx);
}

bool mcld::LLVMTargetMachine::addPassesToEmitFile(PassManagerBase &pPM,
                                         formatted_raw_ostream &pOS,
                                         mcld::CodeGenFileType pFileType,
                                         CodeGenOpt::Level pOptLvl,
                                         bool pDisableVerify ) {

  MCContext* Context = 0;
  if (addCommonCodeGenPasses(pPM, pOptLvl, pDisableVerify, Context))
    return true;
  assert(Context != 0 && "Failed to get MCContext");

  if (getTM().hasMCSaveTempLabels())
    Context->setAllowTemporaryLabels(false);

  const MCAsmInfo &MAI = *getTM().getMCAsmInfo();
  MCCodeEmitter *MCE = 0;
  TargetAsmBackend *TAB = 0;
  MachineFunctionPass *funcPass = 0;
  OwningPtr<MCStreamer> AsmStreamer;

  switch( pFileType ) {
  default:
  case mcld::CGFT_NULLFile: 
    assert(0 && "fatal: file type is not set!");
    break;
  case CGFT_ASMFile: {
    MCInstPrinter *InstPrinter =
      getTarget().get()->createMCInstPrinter(MAI.getAssemblerDialect(), MAI);

    // Create a code emitter if asked to show the encoding.
    if (ArgShowMCEncoding) {
#if LLVM_VERSION > 2
      const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
      MCE = getTarget().get()->createCodeEmitter(*(getTM().getInstrInfo()), STI, *Context);
#else
      MCE = getTarget().get()->createCodeEmitter(getTM(), *Context);
#endif
      TAB = getTarget().get()->createAsmBackend(m_Triple);
      if (MCE == 0 || TAB == 0)
        return true;
    }

    MCStreamer *S = getTarget().get()->createAsmStreamer(*Context, pOS,
                                                         ArgAsmVerbose,
                                                         getTM().hasMCUseLoc(),
                                                         getTM().hasMCUseCFI(),
                                                         InstPrinter,
                                                         MCE, TAB,
                                                         ArgShowMCInst);
    AsmStreamer.reset(S);
    funcPass = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer.get());
    if (funcPass == 0)
      return true;
    // If successful, createAsmPrinter took ownership of AsmStreamer
    AsmStreamer.take();
    pPM.add(funcPass);
    break;
  }
  case CGFT_OBJFile: {
    // Create the code emitter for the target if it exists.  If not, .o file
    // emission fails.
#if LLVM_VERSION > 2
      const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
      MCE = getTarget().get()->createCodeEmitter(*getTM().getInstrInfo(), STI, *Context);
#else
    MCE = getTarget().get()->createCodeEmitter(getTM(), *Context);
#endif
    TAB = getTarget().get()->createAsmBackend(m_Triple);
    if (MCE == 0 || TAB == 0)
      return true;

    AsmStreamer.reset(getTarget().get()->createObjectStreamer(m_Triple, *Context,
                                                       *TAB, pOS, MCE,
                                                       getTM().hasMCRelaxAll(),
                                                       getTM().hasMCNoExecStack()));
    AsmStreamer.get()->InitSections();
    funcPass = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer.get());
    if (funcPass == 0)
      return true;
    // If successful, createAsmPrinter took ownership of AsmStreamer
    AsmStreamer.take();
    pPM.add(funcPass);
    break;
  }
  case CGFT_ARCFile: {
    assert(0 && "Output to archive file has not been supported yet!");
    break;
  }
  case CGFT_EXEFile: {
    assert(0 && "Output to executable file will be supported soon.");
    break;
  }
  case CGFT_DSOFile: {
    // Create the code emitter for the target if it exists.  If not, .o file
    // emission fails.
#if LLVM_VERSION > 2
    const MCSubtargetInfo &STI = getTM().getSubtarget<MCSubtargetInfo>();
    MCE = getTarget().get()->createCodeEmitter(*getTM().getInstrInfo(), STI, *Context);
#else
    MCE = getTarget().get()->createCodeEmitter(getTM(), *Context);
#endif
    TAB = getTarget().get()->createAsmBackend(m_Triple);
    if (MCE == 0 || TAB == 0)
      return true;

    MCStreamer* AsmStreamer = getTarget().get()->createObjectStreamer(m_Triple, 
                                                              *Context,
                                                              *TAB,
                                                              llvm::nulls(),
                                                              MCE,
                                                              getTM().hasMCRelaxAll(),
                                                              getTM().hasMCNoExecStack());

    MCLDInfo* ldInfo = getTarget().createLDInfo(m_Triple);

    MCAsmObjectReader *objReader = new MCAsmObjectReader(
                                             *static_cast<MCObjectStreamer*>(AsmStreamer),
                                             *ldInfo);

    AsmStreamer->InitSections();
    AsmPrinter* printer = getTarget().get()->createAsmPrinter(getTM(), *AsmStreamer);
    if (0 == printer )
      return true;
    pPM.add(printer);

    TargetLDBackend* ldBackend = getTarget().createLDBackend(*getTarget().get(), m_Triple);
    if (0 == ldBackend)
      return true;
    funcPass = getTarget().createSectLinker(m_Triple, *ldInfo, *ldBackend);
    if (0 == funcPass)
      return true;
    pPM.add(funcPass);
    break;
  }
  } // switch

  setCodeModelForStatic();
  pPM.add(createGCInfoDeleter()); // not in addPassesToMC
  return false;
}

