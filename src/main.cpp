/*****************************************************************************
 *   The BOLD Project, Copyright (C), 2011 -                                 *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@gmail.com>                                          *
 *   Jush Lu <jush.msn@gmail.com>                                            *
 ****************************************************************************/
#include <llvm/Support/CommandLine.h>
#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Support/IRReader.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetRegistry.h>
#include <llvm/Target/TargetSelect.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/LLVMContext.h>
#include <llvm/Support/Host.h>
#include <mcld/Target/TargetMachine.h>
#include <mcld/Target/TargetSelect.h>
#include <mcld/Target/TargetRegistry.h>

#if LLVM_VERSION > 2
#include <llvm/MC/SubtargetFeature.h>
#else
#include <llvm/Target/SubtargetFeature.h>
#endif

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace llvm;

#ifdef ENABLE_UNITTEST
#include <gtest.h>

static cl::opt<bool>
UnitTest("unittest",  cl::desc("do unit test") );



int unit_test( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

#endif


// General options for llc.  Other pass-specific options are specified
// within the corresponding llc passes, and target-specific options
// and back-end code generation options are specified with the target machine.
//
static cl::opt<std::string>
InputFilename("dB",
              cl::desc("set default bitcode"),
              cl::value_desc("bitcode"),
              cl::init("-"));

static cl::opt<std::string>
OutputFilename("o",
               cl::desc("Output filename"),
               cl::value_desc("filename"));

// Determine optimization level.
static cl::opt<char>
OptLevel("O",
         cl::desc("Optimization level. [-O0, -O1, -O2, or -O3] "
                  "(default = '-O2')"),
         cl::Prefix,
         cl::ZeroOrMore,
         cl::init(' '));

static cl::opt<std::string>
TargetTriple("mtriple", cl::desc("Override target triple for module"));

static cl::opt<std::string>
MArch("march", cl::desc("Architecture to generate code for (see --version)"));

static cl::opt<std::string>
MCPU("mcpu",
  cl::desc("Target a specific cpu type (-mcpu=help for details)"),
  cl::value_desc("cpu-name"),
  cl::init(""));

static cl::list<std::string>
MAttrs("mattr",
  cl::CommaSeparated,
  cl::desc("Target specific attributes (-mattr=help for details)"),
  cl::value_desc("a1,+a2,-a3,..."));

cl::opt<mcld::CodeGenFileType>
FileType("filetype", cl::init(mcld::CGFT_EXEFile),
  cl::desc("Choose a file type (not all types are supported by all targets):"),
  cl::values(
       clEnumValN(mcld::CGFT_ASMFile, "asm",
                  "Emit an assembly ('.s') file"),
       clEnumValN(mcld::CGFT_OBJFile, "obj",
                  "Emit a relocatable object ('.o') file"),
       clEnumValN(mcld::CGFT_ARCFile, "arc",
                  "Emit an archive ('.a') file"),
       clEnumValN(mcld::CGFT_DSOFile, "dso",
                  "Emit an dynamic shared object ('.so') file"),
       clEnumValN(mcld::CGFT_EXEFile, "exe",
                  "Emit a executable ('.exe') file"),
       clEnumValN(mcld::CGFT_NULLFile, "null",
                  "Emit nothing, for performance testing"),
       clEnumValEnd));

cl::opt<bool> NoVerify("disable-verify", cl::Hidden,
                       cl::desc("Do not verify input module"));

//===----------------------------------------------------------------------===//
/// non-member functions

// GetFileNameRoot - Helper function to get the basename of a filename.
static inline void
GetFileNameRoot(const std::string &pInputFilename, std::string& pFileNameRoot)
{
  std::string outputFilename;
  /* *** */
  const std::string& IFN = InputFilename;
  int Len = IFN.length();
  if ((Len > 2) &&
      IFN[Len-3] == '.' &&
      ((IFN[Len-2] == 'b' && IFN[Len-1] == 'c') ||
       (IFN[Len-2] == 'l' && IFN[Len-1] == 'l'))) 
    pFileNameRoot = std::string(IFN.begin(), IFN.end()-3); // s/.bc/.s/
  else
    pFileNameRoot = std::string(IFN);
}

static void GetOutputName(const char* pTargetName,
                          Triple::OSType pOSType,
                          mcld::CodeGenFileType pFileType,
                          const std::string& pInputFilename,
                          std::string& pOutputFilename)
{
  // If we don't yet have an output filename, make one.
  if (pOutputFilename.empty()) {
    if (pInputFilename == "-")
      pOutputFilename = "-";
    else {
      GetFileNameRoot(pInputFilename, pOutputFilename);

      switch (pFileType) {
      case mcld::CGFT_ASMFile:
        if (pTargetName[0] == 'c') {
          if (pTargetName[1] == 0)
            pOutputFilename += ".cbe.c";
          else if (pTargetName[1] == 'p' && pTargetName[2] == 'p')
            pOutputFilename += ".cpp";
          else
            pOutputFilename += ".s";
        } 
        else
          pOutputFilename += ".s";
        break;
      case mcld::CGFT_OBJFile:
        if (pOSType == Triple::Win32)
          pOutputFilename += ".obj";
        else
          pOutputFilename += ".o";
        break;
      case mcld::CGFT_DSOFile:
        if (pOSType == Triple::Win32)
         pOutputFilename += ".dll";
        else
         pOutputFilename += ".so";
        break;
      case mcld::CGFT_ARCFile:
         pOutputFilename += ".a";
        break;
      case mcld::CGFT_EXEFile:
      case mcld::CGFT_NULLFile:
        // do nothing
        break;
      default:
        assert(0 && "Unknown file type");
      }
    }
  }
}

int main( int argc, char* argv[] )
{
  
  LLVMContext &Context = getGlobalContext();
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  // Initialize targets first, so that --version shows registered targets.
  InitializeAllTargets();
  InitializeAllAsmPrinters();
  InitializeAllAsmParsers();
  InitializeAllTargetMCs();
  mcld::InitializeAllTargets();
  mcld::InitializeAllLinkers();
  cl::ParseCommandLineOptions(argc, argv, "llvm MCLinker\n");

#ifdef ENABLE_UNITTEST
  if (UnitTest) {
    return unit_test( argc, argv );
  }
#endif

  // Load the module to be compiled...
  SMDiagnostic Err;
  std::auto_ptr<Module> M;

  M.reset(ParseIRFile(InputFilename, Err, Context));
  if (M.get() == 0) {
    Err.Print(argv[0], errs());
    errs() << "** First positional argument must be a bitcode/llvm asm file. **\n";
    return 1;
  }
  Module &mod = *M.get();

  // If we are supposed to override the target triple, do so now.
  if (!TargetTriple.empty())
    mod.setTargetTriple(Triple::normalize(TargetTriple));

  Triple TheTriple(mod.getTargetTriple());
  if (TheTriple.getTriple().empty())
    TheTriple.setTriple(sys::getHostTriple());

  // Allocate target machine.  First, check whether the user has explicitly
  // specified an architecture to compile for. If so we have to look it up by
  // name, because it might be a backend that has no mapping to a target triple.
  const mcld::Target *TheTarget = 0;
  if (!MArch.empty()) {
    for (mcld::TargetRegistry::iterator it = mcld::TargetRegistry::begin(),
           ie = mcld::TargetRegistry::end(); it != ie; ++it) {
      if (MArch == (*it)->get()->getName()) {
        TheTarget = *it;
        break;
      }
    }

    if (!TheTarget) {
      errs() << argv[0] << ": error: invalid target '" << MArch << "'.\n";
      return 1;
    }

    // Adjust the triple to match (if known), otherwise stick with the
    // module/host triple.
    Triple::ArchType Type = Triple::getArchTypeForLLVMName(MArch);
    if (Type != Triple::UnknownArch)
      TheTriple.setArch(Type);
  }
  else {
    std::string Err;
    TheTarget = mcld::TargetRegistry::lookupTarget(TheTriple.getTriple(), Err);
    if (TheTarget == 0) {
      errs() << argv[0] << ": error auto-selecting target for module '"
             << Err << "'.  Please use the -march option to explicitly "
             << "pick a target.\n";
      return 1;
    }
  }

  // Package up features to be passed to target/subtarget
  std::string FeaturesStr;
#if LLVM_VERSION > 2
  if (MAttrs.size()) {
    SubtargetFeatures Features;
    for (unsigned i = 0; i != MAttrs.size(); ++i)
      Features.AddFeature(MAttrs[i]);
    FeaturesStr = Features.getString();
  }
#else
  if (MCPU.size() || MAttrs.size()) {
    SubtargetFeatures Features;
    Features.setCPU(MCPU);
    for (unsigned i = 0; i != MAttrs.size(); ++i)
      Features.AddFeature(MAttrs[i]);
    FeaturesStr = Features.getString();
  }
#endif

  std::auto_ptr<mcld::LLVMTargetMachine> target_machine( 
          TheTarget->createTargetMachine(TheTriple.getTriple(), 
#if LLVM_VERSION > 2
                                         MCPU,
#endif
                                         FeaturesStr));
  assert(target_machine.get() && "Could not allocate target machine!");
  mcld::LLVMTargetMachine &TheTargetMachine = *target_machine.get();

#if LLVM_VERSION > 2
  TheTargetMachine.getTM().setMCUseLoc(false);
  TheTargetMachine.getTM().setMCUseCFI(false);
#endif

  // get Output Filename
  GetOutputName(TheTarget->get()->getName(),
                TheTriple.getOS(),
                FileType,
                InputFilename,
                OutputFilename);

  CodeGenOpt::Level OLvl = CodeGenOpt::Default;
  switch (OptLevel) {
  default:
    errs() << argv[0] << ": invalid optimization level.\n";
    return 1;
  case ' ': break;
  case '0': OLvl = CodeGenOpt::None; break;
  case '1': OLvl = CodeGenOpt::Less; break;
  case '2': OLvl = CodeGenOpt::Default; break;
  case '3': OLvl = CodeGenOpt::Aggressive; break;
  }

  // Build up all of the passes that we want to do to the module.
  PassManager PM;

  // Add the target data from the target machine, if it exists, or the module.
  if (const TargetData *TD = TheTargetMachine.getTM().getTargetData())
    PM.add(new TargetData(*TD));
  else
    PM.add(new TargetData(&mod));

  // Override default to generate verbose assembly.
  TheTargetMachine.getTM().setAsmVerbosityDefault(true);

  {
    // Ask the target to add backend passes as necessary.
    if( TheTargetMachine.addPassesToEmitFile(PM,
                                             InputFilename,
                                             OutputFilename,
                                             FileType,
                                             OLvl,
                                             NoVerify)) {
      errs() << argv[0] << ": target does not support generation of this"
             << " file type!\n";
      return 1;
    }

#if LLVM_VERSION > 2
    // Before executing passes, print the final values of the LLVM options.
    cl::PrintOptionValues();
#endif

    PM.run(mod);
  }
}

