//===- llvm-mcld.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/TargetMachine.h>
#include <mcld/Support/TargetSelect.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/Support/DerivedPositionDependentOptions.h>
#include <mcld/Support/RealPath.h>
#include <mcld/CodeGen/SectLinkerOption.h>

#include <llvm/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Pass.h>
#include <llvm/ADT/Triple.h>
#include <llvm/LLVMContext.h>
#include <llvm/MC/SubtargetFeature.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Debug.h>
#include <llvm/Support/FormattedStream.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/IRReader.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Target/TargetData.h>
#include <llvm/Target/TargetMachine.h>

using namespace llvm;

#ifdef ENABLE_UNITTEST
#include <gtest.h>

static cl::opt<bool>
UnitTest("unittest",  cl::desc("do unit test") );

int unit_test( int argc, char* argv[] )
{
  testing::InitGoogleTest( &argc, argv );
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
              cl::value_desc("bitcode"));

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

static cl::opt<Reloc::Model>
RelocModel("relocation-model",
             cl::desc("Choose relocation model"),
             cl::init(Reloc::Default),
             cl::values(
            clEnumValN(Reloc::Default, "default",
                       "Target default relocation model"),
            clEnumValN(Reloc::Static, "static",
                       "Non-relocatable code"),
            clEnumValN(Reloc::PIC_, "pic",
                       "Fully relocatable, position independent code"),
            clEnumValN(Reloc::DynamicNoPIC, "dynamic-no-pic",
                       "Relocatable external references, non-relocatable code"),
            clEnumValEnd));

static cl::opt<llvm::CodeModel::Model>
CMModel("code-model",
        cl::desc("Choose code model"),
        cl::init(CodeModel::Default),
        cl::values(clEnumValN(CodeModel::Default, "default",
                              "Target default code model"),
                   clEnumValN(CodeModel::Small, "small",
                              "Small code model"),
                   clEnumValN(CodeModel::Kernel, "kernel",
                              "Kernel code model"),
                   clEnumValN(CodeModel::Medium, "medium",
                              "Medium code model"),
                   clEnumValN(CodeModel::Large, "large",
                              "Large code model"),
                   clEnumValEnd));

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

static cl::opt<bool>
EnableFPMAD("enable-fp-mad",
  cl::desc("Enable less precise MAD instructions to be generated"),
  cl::init(false));

static cl::opt<bool>
PrintCode("print-machineinstrs",
  cl::desc("Print generated machine code"),
  cl::init(false));

static cl::opt<bool>
DisableFPElim("disable-fp-elim",
  cl::desc("Disable frame pointer elimination optimization"),
  cl::init(false));

static cl::opt<bool>
DisableFPElimNonLeaf("disable-non-leaf-fp-elim",
  cl::desc("Disable frame pointer elimination optimization for non-leaf funcs"),
  cl::init(false));

static cl::opt<bool>
DisableExcessPrecision("disable-excess-fp-precision",
  cl::desc("Disable optimizations that may increase FP precision"),
  cl::init(false));

static cl::opt<bool>
EnableUnsafeFPMath("enable-unsafe-fp-math",
  cl::desc("Enable optimizations that may decrease FP precision"),
  cl::init(false));

static cl::opt<bool>
EnableNoInfsFPMath("enable-no-infs-fp-math",
  cl::desc("Enable FP math optimizations that assume no +-Infs"),
  cl::init(false));

static cl::opt<bool>
EnableNoNaNsFPMath("enable-no-nans-fp-math",
  cl::desc("Enable FP math optimizations that assume no NaNs"),
  cl::init(false));

static cl::opt<bool>
EnableHonorSignDependentRoundingFPMath("enable-sign-dependent-rounding-fp-math",
  cl::Hidden,
  cl::desc("Force codegen to assume rounding mode can change dynamically"),
  cl::init(false));

static cl::opt<bool>
GenerateSoftFloatCalls("soft-float",
  cl::desc("Generate software floating point library calls"),
  cl::init(false));

static cl::opt<llvm::FloatABI::ABIType>
FloatABIForCalls("float-abi",
  cl::desc("Choose float ABI type"),
  cl::init(FloatABI::Default),
  cl::values(
    clEnumValN(FloatABI::Default, "default",
               "Target default float ABI type"),
    clEnumValN(FloatABI::Soft, "soft",
               "Soft float ABI (implied by -soft-float)"),
    clEnumValN(FloatABI::Hard, "hard",
               "Hard float ABI (uses FP registers)"),
    clEnumValEnd));

static cl::opt<bool>
DontPlaceZerosInBSS("nozero-initialized-in-bss",
  cl::desc("Don't place zero-initialized symbols into bss section"),
  cl::init(false));

static cl::opt<bool>
EnableJITExceptionHandling("jit-enable-eh",
  cl::desc("Emit exception handling information"),
  cl::init(false));

// In debug builds, make this default to true.
#ifdef NDEBUG
#define EMIT_DEBUG false
#else
#define EMIT_DEBUG true
#endif
static cl::opt<bool>
EmitJitDebugInfo("jit-emit-debug",
  cl::desc("Emit debug information to debugger"),
  cl::init(EMIT_DEBUG));
#undef EMIT_DEBUG

static cl::opt<bool>
EmitJitDebugInfoToDisk("jit-emit-debug-to-disk",
  cl::Hidden,
  cl::desc("Emit debug info objfiles to disk"),
  cl::init(false));

static cl::opt<bool>
EnableGuaranteedTailCallOpt("tailcallopt",
  cl::desc("Turn fastcc calls into tail calls by (potentially) changing ABI."),
  cl::init(false));

static cl::opt<unsigned>
OverrideStackAlignment("stack-alignment",
  cl::desc("Override default stack alignment"),
  cl::init(0));

static cl::opt<bool>
EnableRealignStack("realign-stack",
  cl::desc("Realign stack if needed"),
  cl::init(true));

static cl::opt<bool>
DisableSwitchTables(cl::Hidden, "disable-jump-tables",
  cl::desc("Do not generate jump tables."),
  cl::init(false));

static cl::opt<std::string>
TrapFuncName("trap-func", cl::Hidden,
  cl::desc("Emit a call to trap function rather than a trap instruction"),
  cl::init(""));

static cl::opt<bool>
SegmentedStacks("segmented-stacks",
  cl::desc("Use segmented stacks if possible."),
  cl::init(false));

//===----------------------------------------------------------------------===//
// Command Line Options
// There are four kinds of command line options:
//   1. input, (may be a file, such as -m and /tmp/XXXX.o.)
//   2. attribute of inputs, (describing the attributes of inputs, such as
//      --as-needed and --whole-archive. usually be positional.)
//   3. scripting options, (represent a subset of link scripting language, such
//      as --defsym.)
//   4. and general options. (the rest of options)
//===----------------------------------------------------------------------===//
// General Options
static cl::opt<mcld::sys::fs::Path, false, llvm::cl::parser<mcld::sys::fs::Path> >
ArgSysRoot("sysroot",
           cl::desc("Use directory as the location of the sysroot, overriding the configure-time default."),
           cl::value_desc("directory"),
           cl::ValueRequired);

static cl::list<mcld::MCLDDirectory, bool, llvm::cl::parser<mcld::MCLDDirectory> >
ArgSearchDirList("L",
                 cl::ZeroOrMore,
                 cl::desc("Add path searchdir to the list of paths that ld will search for archive libraries and ld control scripts."),
                 cl::value_desc("searchdir"),
                 cl::Prefix);

static cl::alias
ArgSearchDirListAlias("library-path",
                      cl::desc("alias for -L"),
                      cl::aliasopt(ArgSearchDirList));

static cl::opt<bool>
ArgTrace("t",
         cl::desc("Print the names of the input files as ld processes them."));

static cl::alias
ArgTraceAlias("trace",
              cl::desc("alias for -t"),
              cl::aliasopt(ArgTrace));

static cl::opt<bool>
ArgVerbose("V",
          cl::desc("Display the version number for ld and list the linker emulations supported."));

static cl::alias
ArgVerboseAlias("verbose",
                cl::desc("alias for -V"),
                cl::aliasopt(ArgVerbose));

static cl::opt<std::string>
ArgEntry("e",
         cl::desc("Use entry as the explicit symbol for beginning execution of your program."),
         cl::value_desc("entry"),
         cl::ValueRequired);

static cl::alias
ArgEntryAlias("entry",
              cl::desc("alias for -e"),
              cl::aliasopt(ArgEntry));

static cl::opt<bool>
ArgBsymbolic("Bsymbolic",
             cl::desc("Bind references within the shared library."),
             cl::init(false));

static cl::opt<std::string>
ArgSOName("soname",
          cl::desc("Set internal name of shared library"),
          cl::value_desc("name"));

//===----------------------------------------------------------------------===//
// Inputs
static cl::list<mcld::sys::fs::Path>
ArgInputObjectFiles(cl::Positional,
                    cl::desc("[input object files]"),
                    cl::ZeroOrMore);

static cl::list<std::string>
ArgNameSpecList("l",
            cl::ZeroOrMore,
            cl::desc("Add the archive or object file specified by namespec to the list of files to link."),
            cl::value_desc("namespec"),
            cl::Prefix);

static cl::alias
ArgNameSpecListAlias("library",
                 cl::desc("alias for -l"),
                 cl::aliasopt(ArgNameSpecList));

static cl::list<bool>
ArgStartGroupList("start-group",
                  cl::ValueDisallowed,
                  cl::desc("start to record a group of archives"));

static cl::alias
ArgStartGroupListAlias("(",
                       cl::desc("alias for --start-group"),
                       cl::aliasopt(ArgStartGroupList));

static cl::list<bool>
ArgEndGroupList("end-group",
                cl::ValueDisallowed,
                cl::desc("stop recording a group of archives"));

static cl::alias
ArgEndGroupListAlias(")",
                     cl::desc("alias for --end-group"),
                     cl::aliasopt(ArgEndGroupList));

//===----------------------------------------------------------------------===//
// Attributes of Inputs
static cl::list<bool>
ArgWholeArchiveList("whole-archive",
                    cl::ValueDisallowed,
                    cl::desc("For each archive mentioned on the command line after the --whole-archive option, include all object files in the archive."));

static cl::list<bool>
ArgNoWholeArchiveList("no-whole-archive",
                    cl::ValueDisallowed,
                    cl::desc("Turn off the effect of the --whole-archive option for subsequent archive files."));

static cl::list<bool>
ArgAsNeededList("as-needed",
                cl::ValueDisallowed,
                cl::desc("This option affects ELF DT_NEEDED tags for dynamic libraries mentioned on the command line after the --as-needed option."));

static cl::list<bool>
ArgNoAsNeededList("no-as-needed",
                cl::ValueDisallowed,
                cl::desc("Turn off the effect of the --as-needed option for subsequent dynamic libraries"));

static cl::list<bool>
ArgAddNeededList("add-needed",
                cl::ValueDisallowed,
                cl::desc("--add-needed causes DT_NEEDED tags are always emitted for those libraries from DT_NEEDED tags. This is the default behavior."));

static cl::list<bool>
ArgNoAddNeededList("no-add-needed",
                cl::ValueDisallowed,
                cl::desc("--no-add-needed causes DT_NEEDED tags will never be emitted for those libraries from DT_NEEDED tags"));

static cl::list<bool>
ArgBDynamicList("Bdynamic",
                cl::ValueDisallowed,
                cl::desc("Link against dynamic library"));

static cl::alias
ArgBDynamicListAlias1("dy",
                     cl::desc("alias for --Bdynamic"),
                     cl::aliasopt(ArgBDynamicList));

static cl::alias
ArgBDynamicListAlias2("call_shared",
                     cl::desc("alias for --Bdynamic"),
                     cl::aliasopt(ArgBDynamicList));

static cl::list<bool>
ArgBStaticList("Bstatic",
                cl::ValueDisallowed,
                cl::desc("Link against static library"));

static cl::alias
ArgBStaticListAlias1("dn",
                     cl::desc("alias for --Bstatic"),
                     cl::aliasopt(ArgBStaticList));

static cl::alias
ArgBStaticListAlias2("static",
                     cl::desc("alias for --Bstatic"),
                     cl::aliasopt(ArgBStaticList));

static cl::alias
ArgBStaticListAlias3("non_shared",
                     cl::desc("alias for --Bstatic"),
                     cl::aliasopt(ArgBStaticList));

//===----------------------------------------------------------------------===//
// Scripting Options


//===----------------------------------------------------------------------===//
/// non-member functions

// GetFileNameRoot - Helper function to get the basename of a filename.
static inline void
GetFileNameRoot(const std::string &pInputFilename, std::string& pFileNameRoot)
{
  std::string outputFilename;
  /* *** */
  const std::string& IFN = pInputFilename;
  int Len = IFN.length();
  if ((Len > 2) &&
      IFN[Len-3] == '.' &&
      ((IFN[Len-2] == 'b' && IFN[Len-1] == 'c') ||
       (IFN[Len-2] == 'l' && IFN[Len-1] == 'l')))
    pFileNameRoot = std::string(IFN.begin(), IFN.end()-3); // s/.bc/.s/
  else
    pFileNameRoot = std::string(IFN);
}

static tool_output_file *GetOutputStream(const char* pTargetName,
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

  // Decide if we need "binary" output.
  bool Binary = false;
  switch (pFileType) {
  default: assert(0 && "Unknown file type");
  case mcld::CGFT_ASMFile:
    break;
  case mcld::CGFT_ARCFile:
  case mcld::CGFT_OBJFile:
  case mcld::CGFT_DSOFile:
  case mcld::CGFT_EXEFile:
  case mcld::CGFT_NULLFile:
    Binary = true;
    break;
  }

  // Open the file.
  std::string error;
  unsigned OpenFlags = 0;
  if (Binary) OpenFlags |= raw_fd_ostream::F_Binary;
  tool_output_file *FDOut = new tool_output_file(pOutputFilename.c_str(), error,
                                                 OpenFlags);
  if (!error.empty()) {
    errs() << error << '\n';
    delete FDOut;
    return 0;
  }

  return FDOut;
}

static bool ProcessLinkerInputsFromCommand(mcld::SectLinkerOption &pOption) {
  // -----  Set up General Options  ----- //
  // set up soname
  pOption.info().output().setSOName(ArgSOName);

  // set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      pOption.info().options().setSysroot(ArgSysRoot);
  }

  // add all search directories
  cl::list<mcld::MCLDDirectory>::iterator sd;
  cl::list<mcld::MCLDDirectory>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (sd->isInSysroot())
      sd->setSysroot(pOption.info().options().sysroot());
    if (exists(sd->path()) && is_directory(sd->path())) {
      pOption.info().options().directories().add(*sd);
    }
    else {
      // FIXME: need a warning function
      errs() << "WARNING: can not open search directory `-L"
             << sd->name()
             << "'.\n";
    }
  }

  pOption.info().options().setTrace(ArgTrace);
  pOption.info().options().setVerbose(ArgVerbose);
  pOption.info().options().setEntry(ArgEntry);
  pOption.info().options().setBsymbolic(ArgBsymbolic);

  // -----  Set up Inputs  ----- //
  // add all start-group
  cl::list<bool>::iterator sg;
  cl::list<bool>::iterator sgEnd = ArgStartGroupList.end();
  for (sg=ArgStartGroupList.begin(); sg!=sgEnd; ++sg) {
    // calculate position
    pOption.appendOption(new mcld::StartGroupOption(
                                    ArgStartGroupList.getPosition(sg-ArgStartGroupList.begin())));
  }

  // add all end-group
  cl::list<bool>::iterator eg;
  cl::list<bool>::iterator egEnd = ArgEndGroupList.end();
  for (eg=ArgEndGroupList.begin(); eg!=egEnd; ++eg) {
    // calculate position
    pOption.appendOption(new mcld::EndGroupOption(
                                    ArgEndGroupList.getPosition(eg-ArgEndGroupList.begin())));
  }

  // add all namespecs
  cl::list<std::string>::iterator ns;
  cl::list<std::string>::iterator nsEnd = ArgNameSpecList.end();
  for (ns=ArgNameSpecList.begin(); ns!=nsEnd; ++ns) {
    // calculate position
    pOption.appendOption(new mcld::NamespecOption(
                                    ArgNameSpecList.getPosition(ns-ArgNameSpecList.begin()),
                                    *ns));
  }

  // add all object files
  cl::list<mcld::sys::fs::Path>::iterator obj;
  cl::list<mcld::sys::fs::Path>::iterator objEnd = ArgInputObjectFiles.end();
  for (obj=ArgInputObjectFiles.begin(); obj!=objEnd; ++obj) {
    // calculate position
    pOption.appendOption(new mcld::InputFileOption(
                                    ArgInputObjectFiles.getPosition(obj-ArgInputObjectFiles.begin()),
                                    *obj));
  }

  // -----  Set up Attributes of Inputs  ----- //
  // --whole-archive
  cl::list<bool>::iterator attr = ArgWholeArchiveList.begin();
  cl::list<bool>::iterator attrEnd = ArgWholeArchiveList.end();
  for (; attr!=attrEnd; ++attr) {
    pOption.appendOption(new mcld::WholeArchiveOption(
                                    ArgWholeArchiveList.getPosition(attr-ArgWholeArchiveList.begin())));
  }

  // --no-whole-archive
  attr = ArgNoWholeArchiveList.begin();
  attrEnd = ArgNoWholeArchiveList.end();
  for (; attr!=attrEnd; ++attr) {
    pOption.appendOption(new mcld::NoWholeArchiveOption(
                                    ArgNoWholeArchiveList.getPosition(attr-ArgNoWholeArchiveList.begin())));
  }

  // --as-needed
  attr = ArgAsNeededList.begin();
  attrEnd = ArgAsNeededList.end();
  while(attr != attrEnd) {
    pOption.appendOption(new mcld::AsNeededOption(
                                    ArgAsNeededList.getPosition(attr-ArgAsNeededList.begin())));
    ++attr;
  }

  // --no-as-needed
  attr = ArgNoAsNeededList.begin();
  attrEnd = ArgNoAsNeededList.end();
  while(attr != attrEnd) {
    pOption.appendOption(new mcld::NoAsNeededOption(
                                    ArgNoAsNeededList.getPosition(attr-ArgNoAsNeededList.begin())));
    ++attr;
  }

  // --add-needed
  attr = ArgAddNeededList.begin();
  attrEnd = ArgAddNeededList.end();
  while(attr != attrEnd) {
    pOption.appendOption(new mcld::AddNeededOption(
                                    ArgAddNeededList.getPosition(attr-ArgAddNeededList.begin())));
    ++attr;
  }

  // --no-add-needed
  attr = ArgNoAddNeededList.begin();
  attrEnd = ArgNoAddNeededList.end();
  while(attr != attrEnd) {
    pOption.appendOption(new mcld::NoAddNeededOption(
                                    ArgNoAddNeededList.getPosition(attr-ArgNoAddNeededList.begin())));
    ++attr;
  }

  // -Bdynamic
  attr = ArgBDynamicList.begin();
  attrEnd = ArgBDynamicList.end();
  while(attr != attrEnd) {
    pOption.appendOption(new mcld::BDynamicOption(
                                    ArgBDynamicList.getPosition(attr-ArgBDynamicList.begin())));
  }

  // -Bstatic
  attr = ArgBStaticList.begin();
  attrEnd = ArgBStaticList.end();
  while(attr != attrEnd) {
    pOption.appendOption(new mcld::BStaticOption(
                                    ArgBStaticList.getPosition(attr-ArgBStaticList.begin())));
    ++attr;
  }

  // -----  Set up Scripting Options  ----- //

  return false;
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
  std::auto_ptr<Module> M;

  if (InputFilename.empty() && (FileType != mcld::CGFT_DSOFile)) {
    // Read from stdin
    InputFilename = "-";
  }

  if (!InputFilename.empty()) {
    SMDiagnostic Err;
    M.reset(ParseIRFile(InputFilename, Err, Context));

    if (M.get() == 0) {
      Err.print(argv[0], errs());
      errs() << "** Failed to to the given bitcode/llvm asm file '"
             << InputFilename << "'. **\n";
      return 1;
    }
  } else {
    // If here, output must be dynamic shared object (mcld::CGFT_DSOFile).

    // Create an empty Module
    M.reset(new Module("Empty Module", Context));
  }
  Module &mod = *M.get();

  // If we are supposed to override the target triple, do so now.
  Triple TheTriple;
  if (!TargetTriple.empty()) {
    TheTriple.setTriple(TargetTriple);
    mod.setTargetTriple(TargetTriple);
  }

  // User doesn't specify the triple from command.
  if (TheTriple.getTriple().empty()) {
    // Try to get one from the input Module.
    const std::string &TripleStr = mod.getTargetTriple();

    if (TripleStr.empty())
      TheTriple.setTriple(sys::getDefaultTargetTriple());
    else
      TheTriple.setTriple(TripleStr);
  }

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
  if (MAttrs.size()) {
    SubtargetFeatures Features;
    for (unsigned i = 0; i != MAttrs.size(); ++i)
      Features.AddFeature(MAttrs[i]);
    FeaturesStr = Features.getString();
  }

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

  TargetOptions Options;
  Options.LessPreciseFPMADOption = EnableFPMAD;
  Options.PrintMachineCode = PrintCode;
  Options.NoFramePointerElim = DisableFPElim;
  Options.NoFramePointerElimNonLeaf = DisableFPElimNonLeaf;
  Options.NoExcessFPPrecision = DisableExcessPrecision;
  Options.UnsafeFPMath = EnableUnsafeFPMath;
  Options.NoInfsFPMath = EnableNoInfsFPMath;
  Options.NoNaNsFPMath = EnableNoNaNsFPMath;
  Options.HonorSignDependentRoundingFPMathOption =
      EnableHonorSignDependentRoundingFPMath;
  Options.UseSoftFloat = GenerateSoftFloatCalls;
  if (FloatABIForCalls != FloatABI::Default)
    Options.FloatABIType = FloatABIForCalls;
  Options.NoZerosInBSS = DontPlaceZerosInBSS;
  Options.JITExceptionHandling = EnableJITExceptionHandling;
  Options.JITEmitDebugInfo = EmitJitDebugInfo;
  Options.JITEmitDebugInfoToDisk = EmitJitDebugInfoToDisk;
  Options.GuaranteedTailCallOpt = EnableGuaranteedTailCallOpt;
  Options.StackAlignmentOverride = OverrideStackAlignment;
  Options.RealignStack = EnableRealignStack;
  Options.DisableJumpTables = DisableSwitchTables;
  Options.TrapFuncName = TrapFuncName;
  Options.EnableSegmentedStacks = SegmentedStacks;

  std::auto_ptr<mcld::LLVMTargetMachine> target_machine(
          TheTarget->createTargetMachine(TheTriple.getTriple(),
                                         MCPU, FeaturesStr, Options,
                                         RelocModel, CMModel, OLvl));
  assert(target_machine.get() && "Could not allocate target machine!");
  mcld::LLVMTargetMachine &TheTargetMachine = *target_machine.get();

  TheTargetMachine.getTM().setMCUseLoc(false);
  TheTargetMachine.getTM().setMCUseCFI(false);

  // Figure out where we are going to send the output...
  OwningPtr<tool_output_file>
  Out(GetOutputStream(TheTarget->get()->getName(),
                      TheTriple.getOS(),
                      FileType,
                      InputFilename,
                      OutputFilename));
  if (!Out) return 1;

  // Build up all of the passes that we want to do to the module.
  PassManager PM;

  // Add the target data from the target machine, if it exists, or the module.
  if (const TargetData *TD = TheTargetMachine.getTM().getTargetData())
    PM.add(new TargetData(*TD));
  else
    PM.add(new TargetData(&mod));

  // Override default to generate verbose assembly.
  TheTargetMachine.getTM().setAsmVerbosityDefault(true);

  // Process the linker input from the command line
  mcld::SectLinkerOption *LinkerOpt =
      new mcld::SectLinkerOption(TheTargetMachine.getLDInfo());

  if (ProcessLinkerInputsFromCommand(*LinkerOpt)) {
    errs() << argv[0] << ": failed to process inputs from command line!\n";
    return 1;
  }

  {
    formatted_raw_ostream FOS(Out->os());

    // Ask the target to add backend passes as necessary.
    if( TheTargetMachine.addPassesToEmitFile(PM,
                                             FOS,
                                             OutputFilename,
                                             FileType,
                                             OLvl,
                                             LinkerOpt,
                                             NoVerify)) {
      errs() << argv[0] << ": target does not support generation of this"
             << " file type!\n";
      return 1;
    }

    // Before executing passes, print the final values of the LLVM options.
    cl::PrintOptionValues();

    PM.run(mod);
  }

  // Declare success.
  Out->keep();

  // clean up
  delete LinkerOpt;

  return 0;
}

