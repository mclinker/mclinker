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
#include <mcld/Support/Path.h>
#include <mcld/Support/RealPath.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LD/DiagnosticLineInfo.h>
#include <mcld/LD/TextDiagnosticPrinter.h>
#include <mcld/MC/MCLDInfo.h>
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
#include <llvm/Support/Signals.h>
#include <llvm/Support/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Support/Process.h>
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

cl::opt<bool> NoVerify("disable-verify", cl::Hidden,
                       cl::desc("Do not verify input module"));

static cl::opt<bool>
EnableFPMAD("enable-fp-mad",
  cl::desc("Enable less precise MAD instructions to be generated"),
  cl::init(false));

static cl::opt<bool>
DisableFPElim("disable-fp-elim",
  cl::desc("Disable frame pointer elimination optimization"),
  cl::init(false));

static cl::opt<bool>
DisableFPElimNonLeaf("disable-non-leaf-fp-elim",
  cl::desc("Disable frame pointer elimination optimization for non-leaf funcs"),
  cl::init(false));

static cl::opt<llvm::FPOpFusion::FPOpFusionMode>
FuseFPOps("fuse-fp-ops",
  cl::desc("Enable aggresive formation of fused FP ops"),
  cl::init(FPOpFusion::Standard),
  cl::values(
    clEnumValN(FPOpFusion::Fast, "fast",
               "Fuse FP ops whenever profitable"),
    clEnumValN(FPOpFusion::Standard, "standard",
               "Only fuse 'blessed' FP ops."),
    clEnumValN(FPOpFusion::Strict, "strict",
               "Only fuse FP ops when the result won't be effected."),
    clEnumValEnd));

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
ArgBitcodeFilename("dB",
              cl::desc("set default bitcode"),
              cl::value_desc("bitcode"));

static cl::opt<mcld::sys::fs::Path, false, llvm::cl::parser<mcld::sys::fs::Path> >
ArgOutputFilename("o",
               cl::desc("Output filename"),
               cl::value_desc("filename"));

static cl::alias
AliasOutputFilename("output",
                    cl::desc("alias for -o"),
                    cl::aliasopt(ArgOutputFilename));

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

static cl::opt<int>
ArgVerbose("verbose",
           cl::init(-1),
           cl::desc("Display the version number for ld and list the linker emulations supported."));

static cl::opt<bool>
ArgVersion("V",
           cl::init(false),
           cl::desc("Display the version number for MCLinker."));

static cl::opt<int>
ArgMaxErrorNum("error-limit",
               cl::init(-1),
               cl::desc("limits the maximum number of erros."));

static cl::opt<int>
ArgMaxWarnNum("warning-limit",
               cl::init(-1),
               cl::desc("limits the maximum number of warnings."));

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

static cl::opt<bool>
ArgBgroup("Bgroup",
          cl::desc("Info the dynamic linker to perform lookups only inside the group."),
          cl::init(false));

static cl::opt<std::string>
ArgSOName("soname",
          cl::desc("Set internal name of shared library"),
          cl::value_desc("name"));

static cl::opt<bool>
ArgNoUndefined("no-undefined",
               cl::desc("Do not allow unresolved references"),
               cl::init(false));

static cl::opt<bool>
ArgAllowMulDefs("allow-multiple-definition",
                cl::desc("Allow multiple definition"),
                cl::init(false));

static cl::opt<bool>
ArgEhFrameHdr("eh-frame-hdr",
              cl::desc("Request creation of \".eh_frame_hdr\" section and ELF \"PT_GNU_EH_FRAME\" segment header."),
              cl::init(false));

static cl::list<mcld::ZOption, bool, llvm::cl::parser<mcld::ZOption> >
ArgZOptionList("z",
               cl::ZeroOrMore,
               cl::desc("The -z options for GNU ld compatibility."),
               cl::value_desc("keyword"),
               cl::Prefix);

cl::opt<mcld::CodeGenFileType>
ArgFileType("filetype", cl::init(mcld::CGFT_EXEFile),
  cl::desc("Choose a file type (not all types are supported by all targets):"),
  cl::values(
       clEnumValN(mcld::CGFT_ASMFile, "asm",
                  "Emit an assembly ('.s') file"),
       clEnumValN(mcld::CGFT_OBJFile, "obj",
                  "Emit a relocatable object ('.o') file"),
       clEnumValN(mcld::CGFT_DSOFile, "dso",
                  "Emit an dynamic shared object ('.so') file"),
       clEnumValN(mcld::CGFT_EXEFile, "exe",
                  "Emit a executable ('.exe') file"),
       clEnumValN(mcld::CGFT_NULLFile, "null",
                  "Emit nothing, for performance testing"),
       clEnumValEnd));

static cl::opt<bool>
ArgShared("shared",
          cl::desc("Create a shared library."),
          cl::init(false));

static cl::alias
ArgSharedAlias("Bshareable",
               cl::desc("alias for -shared"),
               cl::aliasopt(ArgShared));

static cl::opt<bool>
ArgPIE("pie",
       cl::desc("Emit a position-independent executable file"),
       cl::init(false));

static cl::opt<Reloc::Model>
ArgRelocModel("relocation-model",
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

static cl::opt<bool>
ArgFPIC("fPIC",
        cl::desc("Set relocation model to pic. The same as -relocation-model=pic."),
        cl::init(false));

static cl::opt<std::string>
ArgDyld("dynamic-linker",
        cl::desc("Set the name of the dynamic linker."),
        cl::value_desc("Program"));

namespace color {
enum Color {
  Never,
  Always,
  Auto
};
} // namespace of color

static cl::opt<color::Color>
ArgColor("color",
  cl::value_desc("WHEN"),
  cl::desc("Surround the result strings with the marker"),
  cl::init(color::Auto),
  cl::values(
    clEnumValN(color::Never, "never",
      "do not surround result strings"),
    clEnumValN(color::Always, "always",
      "always surround result strings, even the output is a plain file"),
    clEnumValN(color::Auto, "auto",
      "surround result strings only if the output is a tty"),
    clEnumValEnd));

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
static cl::list<std::string>
ArgWrapList("wrap",
            cl::ZeroOrMore,
            cl::desc("Use a wrap function fo symbol."),
            cl::value_desc("symbol"));

static cl::list<std::string>
ArgPortList("portable",
            cl::ZeroOrMore,
            cl::desc("Use a portable function fo symbol."),
            cl::value_desc("symbol"));

//===----------------------------------------------------------------------===//
/// non-member functions

/// GetOutputStream - get the output stream.
static tool_output_file *GetOutputStream(const char* pTargetName,
                                         Triple::OSType pOSType,
                                         mcld::CodeGenFileType pFileType,
                                         const mcld::sys::fs::Path& pInputFilename,
                                         mcld::sys::fs::Path& pOutputFilename)
{
  if (pOutputFilename.empty()) {
    if (0 == pInputFilename.native().compare("-"))
      pOutputFilename.assign("-");
    else {
      switch(pFileType) {
      case mcld::CGFT_ASMFile: {
        if (0 == pInputFilename.native().compare("-"))
          pOutputFilename.assign("_out");
        else
          pOutputFilename.assign(pInputFilename.stem().native());

        if (0 == strcmp(pTargetName, "c"))
          pOutputFilename.native() += ".cbe.c";
        else if (0 == strcmp(pTargetName, "cpp"))
          pOutputFilename.native() += ".cpp";
        else
          pOutputFilename.native() += ".s";
      }
      break;

      case mcld::CGFT_OBJFile: {
        if (0 == pInputFilename.native().compare("-"))
          pOutputFilename.assign("_out");
        else
          pOutputFilename.assign(pInputFilename.stem().native());

        if (pOSType == Triple::Win32)
          pOutputFilename.native() += ".obj";
        else
          pOutputFilename.native() += ".o";
      }
      break;

      case mcld::CGFT_DSOFile: {
        if (Triple::Win32 == pOSType) {
          if (0 == pInputFilename.native().compare("-"))
            pOutputFilename.assign("_out");
          else
            pOutputFilename.assign(pInputFilename.stem().native());
          pOutputFilename.native() += ".dll";
        }
        else
          pOutputFilename.assign("a.out");
      }
      break;

      case mcld::CGFT_EXEFile: {
        if (Triple::Win32 == pOSType) {
          if (0 == pInputFilename.native().compare("-"))
            pOutputFilename.assign("_out");
          else
            pOutputFilename.assign(pInputFilename.stem().native());
          pOutputFilename.native() += ".exe";
        }
        else
          pOutputFilename.assign("a.out");
      }
      break;

      case mcld::CGFT_NULLFile:
        break;
      default:
        llvm::report_fatal_error("Unknown output file type.\n");
      } // end of switch
    } // end of ! pInputFilename == "-"
  } // end of if empty pOutputFilename

  // Decide if we need "binary" output.
  unsigned int fd_flags = 0x0;
  switch (pFileType) {
  default: assert(0 && "Unknown file type");
  case mcld::CGFT_ASMFile:
    break;
  case mcld::CGFT_OBJFile:
  case mcld::CGFT_DSOFile:
  case mcld::CGFT_EXEFile:
  case mcld::CGFT_NULLFile:
    fd_flags |= raw_fd_ostream::F_Binary;
    break;
  }

  // Open the file.
  std::string err_mesg;
  tool_output_file *result_output =
                            new tool_output_file(pOutputFilename.c_str(),
                                                 err_mesg,
                                                 fd_flags);
  if (!err_mesg.empty()) {
    errs() << err_mesg << '\n';
    delete result_output;
    return NULL;
  }

  return result_output;
}

static bool ShouldColorize()
{
   const char* term = getenv("TERM");
   return term && (0 != strcmp(term, "dumb"));
}

static bool ProcessLinkerOptionsFromCommand(mcld::MCLDInfo& pLDInfo) {
  // -----  Set up General Options  ----- //
  // set up soname
  pLDInfo.output().setSOName(ArgSOName);

  // set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      pLDInfo.options().setSysroot(ArgSysRoot);
  }

  // add all search directories
  cl::list<mcld::MCLDDirectory>::iterator sd;
  cl::list<mcld::MCLDDirectory>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (sd->isInSysroot())
      sd->setSysroot(pLDInfo.options().sysroot());
    if (exists(sd->path()) && is_directory(sd->path())) {
      pLDInfo.options().directories().add(*sd);
    }
    else {
      // FIXME: need a warning function
      errs() << "WARNING: can not open search directory `-L"
             << sd->name()
             << "'.\n";
    }
  }

  pLDInfo.options().setPIE(ArgPIE);
  pLDInfo.options().setTrace(ArgTrace);
  pLDInfo.options().setVerbose(ArgVerbose);
  pLDInfo.options().setMaxErrorNum(ArgMaxErrorNum);
  pLDInfo.options().setMaxWarnNum(ArgMaxWarnNum);
  pLDInfo.options().setEntry(ArgEntry);
  pLDInfo.options().setBsymbolic(ArgBsymbolic);
  pLDInfo.options().setBgroup(ArgBgroup);
  pLDInfo.options().setDyld(ArgDyld);
  pLDInfo.options().setNoUndefined(ArgNoUndefined);
  pLDInfo.options().setMulDefs(ArgAllowMulDefs);
  pLDInfo.options().setEhFrameHdr(ArgEhFrameHdr);

  // set up rename map, for --wrap
  cl::list<std::string>::iterator wname;
  cl::list<std::string>::iterator wnameEnd = ArgWrapList.end();
  for (wname = ArgWrapList.begin(); wname != wnameEnd; ++wname) {
    bool exist = false;

    // add wname -> __wrap_wname
    mcld::StringEntry<llvm::StringRef>* to_wrap =
                    pLDInfo.scripts().renameMap().insert(*wname, exist);

    std::string to_wrap_str = "__wrap_" + *wname;
    to_wrap->setValue(to_wrap_str);

    if (exist)
      mcld::warning(mcld::diag::rewrap) << *wname << to_wrap_str;

    // add __real_wname -> wname
    std::string from_real_str = "__real_" + *wname;
    mcld::StringEntry<llvm::StringRef>* from_real =
             pLDInfo.scripts().renameMap().insert(from_real_str, exist);
    from_real->setValue(*wname);
    if (exist)
      mcld::warning(mcld::diag::rewrap) << *wname << from_real_str;
  } // end of for

  // set up rename map, for --portable
  cl::list<std::string>::iterator pname;
  cl::list<std::string>::iterator pnameEnd = ArgPortList.end();
  for (pname = ArgPortList.begin(); pname != pnameEnd; ++pname) {
    bool exist = false;

    // add pname -> pname_portable
    mcld::StringEntry<llvm::StringRef>* to_port =
                  pLDInfo.scripts().renameMap().insert(*pname, exist);

    std::string to_port_str = *pname + "_portable";
    to_port->setValue(to_port_str);

    if (exist)
      mcld::warning(mcld::diag::rewrap) << *pname << to_port_str;

    // add __real_pname -> pname
    std::string from_real_str = "__real_" + *pname;
    mcld::StringEntry<llvm::StringRef>* from_real =
             pLDInfo.scripts().renameMap().insert(from_real_str, exist);

    from_real->setValue(*pname);
    if (exist)
      mcld::warning(mcld::diag::rewrap) << *pname << from_real_str;
  } // end of for

  // set up colorize
  switch (ArgColor) {
    case color::Never:
      pLDInfo.options().setColor(false);
    break;
    case color::Always:
      pLDInfo.options().setColor(true);
    break;
    case color::Auto:
      bool color_option = ShouldColorize() &&
                 llvm::sys::Process::FileDescriptorIsDisplayed(STDOUT_FILENO);
      pLDInfo.options().setColor(color_option);
    break;
  }

  // add -z options
  cl::list<mcld::ZOption>::iterator zOpt;
  cl::list<mcld::ZOption>::iterator zOptEnd = ArgZOptionList.end();
  for (zOpt = ArgZOptionList.begin(); zOpt != zOptEnd; ++zOpt) {
    pLDInfo.options().addZOption(*zOpt);
  }

  // -----  Set up Script Options  ----- //

  return true;
}

static bool ProcessLinkerInputsFromCommand(mcld::SectLinkerOption &pOption) {
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

  return true;
}

int main( int argc, char* argv[] )
{
  LLVMContext &Context = getGlobalContext();
  llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  cl::ParseCommandLineOptions(argc, argv, "MCLinker\n");

#ifdef ENABLE_UNITTEST
  if (UnitTest) {
    return unit_test( argc, argv );
  }
#endif

  // Initialize targets first, so that --version shows registered targets.
  InitializeAllTargets();
  InitializeAllAsmPrinters();
  InitializeAllAsmParsers();
  InitializeAllTargetMCs();
  mcld::InitializeAllTargets();
  mcld::InitializeAllLinkers();
  mcld::InitializeAllDiagnostics();

  // Load the module to be compiled...
  std::auto_ptr<Module> M;

  // -shared
  if (true == ArgShared) {
    ArgFileType = mcld::CGFT_DSOFile;
  }

  // -V
  if (ArgVersion) {
    outs() << "MCLinker - ";
    outs() << mcld::MCLDInfo::version();
    outs() << "\n";
  }

  if (ArgBitcodeFilename.empty() &&
      (mcld::CGFT_DSOFile != ArgFileType &&
       mcld::CGFT_EXEFile != ArgFileType)) {
    // If the file is not given, forcefully read from stdin
    if (ArgVerbose >= 0) {
      errs() << "** The bitcode/llvm asm file is not given. Read from stdin.\n"
             << "** Specify input bitcode/llvm asm file by\n\n"
             << "          llvm-mcld -dB [the bitcode/llvm asm]\n\n";
    }

    ArgBitcodeFilename.assign("-");
  }

  if (!ArgBitcodeFilename.empty()) {
    SMDiagnostic Err;
    M.reset(ParseIRFile(ArgBitcodeFilename.native(), Err, Context));

    if (M.get() == 0) {
      Err.print(argv[0], errs());
      errs() << "** Failed to to the given bitcode/llvm asm file '"
             << ArgBitcodeFilename.native() << "'. **\n";
      return 1;
    }
  }
  else {
    // If here, output must be dynamic shared object (mcld::CGFT_DSOFile) and
    // executable file (mcld::CGFT_EXEFile).
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
      errs() << "error: auto-selecting target `" << TheTriple.getTriple()
             << "'\n"
             << "Please use the -march option to explicitly select a target.\n"
             << "Example:\n"
             << "  $ " << argv[0] << " -march=arm\n";
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

  // set -fPIC
  if (ArgFPIC)
    ArgRelocModel = Reloc::PIC_;

  TargetOptions Options;
  Options.LessPreciseFPMADOption = EnableFPMAD;
  Options.NoFramePointerElim = DisableFPElim;
  Options.NoFramePointerElimNonLeaf = DisableFPElimNonLeaf;
  Options.AllowFPOpFusion = FuseFPOps;
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
  Options.TrapFuncName = TrapFuncName;
  Options.EnableSegmentedStacks = SegmentedStacks;

  std::auto_ptr<mcld::LLVMTargetMachine> target_machine(
          TheTarget->createTargetMachine(TheTriple.getTriple(),
                                         MCPU, FeaturesStr, Options,
                                         ArgRelocModel, CMModel, OLvl));
  assert(target_machine.get() && "Could not allocate target machine!");
  mcld::LLVMTargetMachine &TheTargetMachine = *target_machine.get();

  TheTargetMachine.getTM().setMCUseLoc(false);
  TheTargetMachine.getTM().setMCUseCFI(false);

  // Set up mcld::outs() and mcld::errs()
  InitializeOStreams(TheTargetMachine.getLDInfo());

  // Set up MsgHandler
  OwningPtr<mcld::DiagnosticLineInfo>
    diag_line_info(TheTarget->createDiagnosticLineInfo(*TheTarget,
                                                       TheTriple.getTriple()));
  OwningPtr<mcld::DiagnosticPrinter>
    diag_printer(new mcld::TextDiagnosticPrinter(mcld::errs(),
                                                TheTargetMachine.getLDInfo()));

  mcld::InitializeDiagnosticEngine(TheTargetMachine.getLDInfo(),
                                   diag_line_info.take(),
                                   diag_printer.get());


  // Figure out where we are going to send the output...
  OwningPtr<tool_output_file>
  Out(GetOutputStream(TheTarget->get()->getName(),
                      TheTriple.getOS(),
                      ArgFileType,
                      ArgBitcodeFilename,
                      ArgOutputFilename));
  if (!Out) {
    // FIXME: show some error message pls.
    return 1;
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

  // Process the linker input from the command line
  mcld::SectLinkerOption *LinkerOpt =
      new mcld::SectLinkerOption(TheTargetMachine.getLDInfo());

  if (!ProcessLinkerOptionsFromCommand(TheTargetMachine.getLDInfo())) {
    errs() << argv[0] << ": failed to process linker options from command line!\n";
    return 1;
  }

  if (!ProcessLinkerInputsFromCommand(*LinkerOpt)) {
    errs() << argv[0] << ": failed to process inputs from command line!\n";
    return 1;
  }

  {
    formatted_raw_ostream FOS(Out->os());

    // Ask the target to add backend passes as necessary.
    if( TheTargetMachine.addPassesToEmitFile(PM,
                                             FOS,
                                             ArgOutputFilename.native(),
                                             ArgFileType,
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

  if (0 != diag_printer->getNumErrors()) {
    // If we reached here, we are failing ungracefully. Run the interrupt handlers
    // to make sure any special cleanups get done, in particular that we remove
    // files registered with RemoveFileOnSignal.
    llvm::sys::RunInterruptHandlers();
    diag_printer->finish();
    exit(1);
  }

  diag_printer->finish();
  return 0;
}

