//===- ld-lite.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LinkerConfig.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/CommandLine.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <string>

//===----------------------------------------------------------------------===//
// General options for llc.  Other pass-specific options are specified
// within the corresponding llc passes, and target-specific options
// and back-end code generation options are specified with the target machine.
//===----------------------------------------------------------------------===//
static llvm::cl::opt<std::string>
TargetTriple("mtriple", llvm::cl::desc("Override target triple for module"));

static llvm::cl::opt<std::string>
MArch("march", llvm::cl::desc("Architecture to generate code for (see --version)"));

static llvm::cl::opt<std::string>
MCPU("mcpu",
  llvm::cl::desc("Target a specific cpu type (-mcpu=help for details)"),
  llvm::cl::value_desc("cpu-name"),
  llvm::cl::init(""));

static llvm::cl::list<std::string>
MAttrs("mattr",
  llvm::cl::CommaSeparated,
  llvm::cl::desc("Target specific attributes (-mattr=help for details)"),
  llvm::cl::value_desc("a1,+a2,-a3,..."));

//===----------------------------------------------------------------------===//
// Command Line Options
// There are four kinds of command line options:
//   1. Bitcode option. Used to represent a bitcode.
//   2. Attribute options. Attributes describes the input file after them. For
//      example, --as-needed affects the input file after this option. Attribute
//      options are not attributes. Attribute options are the options that is
//      used to define a legal attribute.
//   3. Scripting options, Used to represent a subset of link scripting
//      language, such as --defsym.
//   4. General options. (the rest of options)
//===----------------------------------------------------------------------===//
// General Options
//===----------------------------------------------------------------------===//
static llvm::cl::opt<mcld::sys::fs::Path, false, llvm::cl::parser<mcld::sys::fs::Path> >
ArgOutputFilename("o",
               llvm::cl::desc("Output filename"),
               llvm::cl::value_desc("filename"));

static llvm::cl::alias
AliasOutputFilename("output",
                    llvm::cl::desc("alias for -o"),
                    llvm::cl::aliasopt(ArgOutputFilename));

static llvm::cl::opt<mcld::sys::fs::Path, false, llvm::cl::parser<mcld::sys::fs::Path> >
ArgSysRoot("sysroot",
           llvm::cl::desc("Use directory as the location of the sysroot, overriding the configure-time default."),
           llvm::cl::value_desc("directory"),
           llvm::cl::ValueRequired);

static llvm::cl::list<std::string, bool, llvm::cl::SearchDirParser>
ArgSearchDirList("L",
                 llvm::cl::ZeroOrMore,
                 llvm::cl::desc("Add path searchdir to the list of paths that ld will search for archive libraries and ld control scripts."),
                 llvm::cl::value_desc("searchdir"),
                 llvm::cl::Prefix);

static llvm::cl::alias
ArgSearchDirListAlias("library-path",
                      llvm::cl::desc("alias for -L"),
                      llvm::cl::aliasopt(ArgSearchDirList));

static llvm::cl::opt<bool>
ArgTrace("t",
         llvm::cl::desc("Print the names of the input files as ld processes them."));

static llvm::cl::alias
ArgTraceAlias("trace",
              llvm::cl::desc("alias for -t"),
              llvm::cl::aliasopt(ArgTrace));

static llvm::cl::opt<int>
ArgVerbose("verbose",
           llvm::cl::init(-1),
           llvm::cl::desc("Display the version number for ld and list the linker emulations supported."));

static llvm::cl::opt<bool>
ArgVersion("V",
           llvm::cl::init(false),
           llvm::cl::desc("Display the version number for MCLinker."));

static llvm::cl::opt<int>
ArgMaxErrorNum("error-limit",
               llvm::cl::init(-1),
               llvm::cl::desc("limits the maximum number of erros."));

static llvm::cl::opt<int>
ArgMaxWarnNum("warning-limit",
               llvm::cl::init(-1),
               llvm::cl::desc("limits the maximum number of warnings."));

static llvm::cl::opt<std::string>
ArgEntry("e",
         llvm::cl::desc("Use entry as the explicit symbol for beginning execution of your program."),
         llvm::cl::value_desc("entry"),
         llvm::cl::ValueRequired);

static llvm::cl::alias
ArgEntryAlias("entry",
              llvm::cl::desc("alias for -e"),
              llvm::cl::aliasopt(ArgEntry));

static llvm::cl::opt<bool>
ArgBsymbolic("Bsymbolic",
             llvm::cl::desc("Bind references within the shared library."),
             llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgBgroup("Bgroup",
          llvm::cl::desc("Info the dynamic linker to perform lookups only inside the group."),
          llvm::cl::init(false));

static llvm::cl::opt<std::string>
ArgSOName("soname",
          llvm::cl::desc("Set internal name of shared library"),
          llvm::cl::value_desc("name"));

static llvm::cl::opt<bool>
ArgNoUndefined("no-undefined",
               llvm::cl::desc("Do not allow unresolved references"),
               llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgAllowMulDefs("allow-multiple-definition",
                llvm::cl::desc("Allow multiple definition"),
                llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgEhFrameHdr("eh-frame-hdr",
              llvm::cl::desc("Request creation of \".eh_frame_hdr\" section and ELF \"PT_GNU_EH_FRAME\" segment header."),
              llvm::cl::init(false));

static llvm::cl::list<mcld::ZOption, bool, llvm::cl::parser<mcld::ZOption> >
ArgZOptionList("z",
               llvm::cl::ZeroOrMore,
               llvm::cl::desc("The -z options for GNU ld compatibility."),
               llvm::cl::value_desc("keyword"),
               llvm::cl::Prefix);

llvm::cl::opt<mcld::LinkerConfig::CodeGenType>
ArgFileType("filetype", llvm::cl::init(mcld::LinkerConfig::Exec),
  llvm::cl::desc("Choose a file type (not all types are supported by all targets):"),
  llvm::cl::values(
       clEnumValN(mcld::LinkerConfig::Object, "obj",
                  "Emit a relocatable object ('.o') file"),
       clEnumValN(mcld::LinkerConfig::DynObj, "dso",
                  "Emit an dynamic shared object ('.so') file"),
       clEnumValN(mcld::LinkerConfig::Exec, "exe",
                  "Emit an executable ('.exe') file"),
       clEnumValN(mcld::LinkerConfig::Binary, "bin",
                  "Emit a binary file"),
       clEnumValEnd));

static llvm::cl::opt<bool>
ArgShared("shared",
          llvm::cl::desc("Create a shared library."),
          llvm::cl::init(false));

static llvm::cl::alias
ArgSharedAlias("Bshareable",
               llvm::cl::desc("alias for -shared"),
               llvm::cl::aliasopt(ArgShared));

static llvm::cl::opt<bool>
ArgPIE("pie",
       llvm::cl::desc("Emit a position-independent executable file"),
       llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgRelocatable("relocatable",
               llvm::cl::desc("Generate relocatable output"),
               llvm::cl::init(false));

static llvm::cl::alias
ArgRelocatableAlias("r",
                    llvm::cl::desc("alias for --relocatable"),
                    llvm::cl::aliasopt(ArgRelocatable));

static llvm::cl::opt<std::string>
ArgDyld("dynamic-linker",
        llvm::cl::ZeroOrMore,
        llvm::cl::desc("Set the name of the dynamic linker."),
        llvm::cl::value_desc("Program"));

namespace color {
enum Color {
  Never,
  Always,
  Auto
};
} // namespace of color

static llvm::cl::opt<color::Color>
ArgColor("color",
  llvm::cl::value_desc("WHEN"),
  llvm::cl::desc("Surround the result strings with the marker"),
  llvm::cl::init(color::Auto),
  llvm::cl::values(
    clEnumValN(color::Never, "never",
      "do not surround result strings"),
    clEnumValN(color::Always, "always",
      "always surround result strings, even the output is a plain file"),
    clEnumValN(color::Auto, "auto",
      "surround result strings only if the output is a tty"),
    clEnumValEnd));

static llvm::cl::opt<bool>
ArgDiscardLocals("discard-locals",
                 llvm::cl::desc("Delete all temporary local symbols."),
                 llvm::cl::init(false));

static llvm::cl::alias
ArgDiscardLocalsAlias("X",
                      llvm::cl::desc("alias for --discard-locals"),
                      llvm::cl::aliasopt(ArgDiscardLocals));

static llvm::cl::opt<bool>
ArgDiscardAll("discard-all",
              llvm::cl::desc("Delete all local symbols."),
              llvm::cl::init(false));

static llvm::cl::alias
ArgDiscardAllAlias("x",
                   llvm::cl::desc("alias for --discard-all"),
                   llvm::cl::aliasopt(ArgDiscardAll));

static llvm::cl::opt<bool>
ArgStripDebug("strip-debug",
              llvm::cl::desc("Omit debugger symbol information from the output file."),
              llvm::cl::init(false));

static llvm::cl::alias
ArgStripDebugAlias("S",
                   llvm::cl::desc("alias for --strip-debug"),
                   llvm::cl::aliasopt(ArgStripDebug));

static llvm::cl::opt<bool>
ArgStripAll("strip-all",
            llvm::cl::desc("Omit all symbol information from the output file."),
            llvm::cl::init(false));

static llvm::cl::alias
ArgStripAllAlias("s",
                 llvm::cl::desc("alias for --strip-all"),
                 llvm::cl::aliasopt(ArgStripAll));

static llvm::cl::opt<bool>
ArgNMagic("nmagic",
          llvm::cl::desc("Do not page align data"),
          llvm::cl::init(false));

static llvm::cl::alias
ArgNMagicAlias("n",
               llvm::cl::desc("alias for --nmagic"),
               llvm::cl::aliasopt(ArgNMagic));

static llvm::cl::opt<bool>
ArgOMagic("omagic",
          llvm::cl::desc("Do not page align data, do not make text readonly"),
          llvm::cl::init(false));

static llvm::cl::alias
ArgOMagicAlias("N",
               llvm::cl::desc("alias for --omagic"),
               llvm::cl::aliasopt(ArgOMagic));


static llvm::cl::opt<int>
ArgGPSize("G",
          llvm::cl::desc("Set the maximum size of objects to be optimized using GP"),
          llvm::cl::init(8));

/// @{
/// @name FIXME: begin of unsupported options
/// @}
static llvm::cl::opt<bool>
ArgGCSections("gc-sections",
              llvm::cl::desc("Enable garbage collection of unused input sections."),
              llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgNoGCSections("no-gc-sections",
              llvm::cl::desc("disable garbage collection of unused input sections."),
              llvm::cl::init(false));

namespace icf {
enum Mode {
  None,
  All,
  Safe
};
} // namespace of icf

static llvm::cl::opt<icf::Mode>
ArgICF("icf",
       llvm::cl::ZeroOrMore,
       llvm::cl::desc("Identical Code Folding"),
       llvm::cl::init(icf::None),
       llvm::cl::values(
         clEnumValN(icf::None, "none",
           "do not perform cold folding"),
         clEnumValN(icf::All, "all",
           "always preform cold folding"),
         clEnumValN(icf::Safe, "safe",
           "Folds ctors, dtors and functions whose pointers are definitely not taken."),
         clEnumValEnd));

// FIXME: add this to target options?
static llvm::cl::opt<bool>
ArgFIXCA8("fix-cortex-a8",
          llvm::cl::desc("Enable Cortex-A8 Thumb-2 branch erratum fix"),
          llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgExportDynamic("export-dynamic",
                 llvm::cl::desc("Export all dynamic symbols"),
                 llvm::cl::init(false));

static llvm::cl::alias
ArgExportDynamicAlias("E",
                      llvm::cl::desc("alias for --export-dynamic"),
                      llvm::cl::aliasopt(ArgExportDynamic));

static llvm::cl::opt<std::string>
ArgEmulation("m",
             llvm::cl::ZeroOrMore,
             llvm::cl::desc("Set GNU linker emulation"),
             llvm::cl::value_desc("emulation"));

static llvm::cl::list<std::string, bool, llvm::cl::SearchDirParser>
ArgRuntimePathLink("rpath-link",
                   llvm::cl::ZeroOrMore,
                   llvm::cl::desc("Add a directory to the link time library search path"),
                   llvm::cl::value_desc("dir"));

static llvm::cl::list<std::string>
ArgExcludeLIBS("exclude-libs",
               llvm::cl::CommaSeparated,
               llvm::cl::desc("Exclude libraries from automatic export"),
               llvm::cl::value_desc("lib1,lib2,..."));

static llvm::cl::opt<std::string>
ArgBuildID("build-id",
           llvm::cl::desc("Request creation of \".note.gnu.build-id\" ELF note section."),
           llvm::cl::value_desc("style"),
           llvm::cl::ValueOptional);

static llvm::cl::opt<std::string>
ArgForceUndefined("u",
                  llvm::cl::desc("Force symbol to be undefined in the output file"),
                  llvm::cl::value_desc("symbol"));

static llvm::cl::alias
ArgForceUndefinedAlias("undefined",
                       llvm::cl::desc("alias for -u"),
                       llvm::cl::aliasopt(ArgForceUndefined));

static llvm::cl::opt<std::string>
ArgVersionScript("version-script",
                 llvm::cl::desc("Version script."),
                 llvm::cl::value_desc("Version script"));

static llvm::cl::opt<bool>
ArgWarnCommon("warn-common",
              llvm::cl::desc("warn common symbol"),
              llvm::cl::init(false));

static llvm::cl::opt<mcld::GeneralOptions::HashStyle>
ArgHashStyle("hash-style", llvm::cl::init(mcld::GeneralOptions::SystemV),
  llvm::cl::desc("Set the type of linker's hash table(s)."),
  llvm::cl::values(
       clEnumValN(mcld::GeneralOptions::SystemV, "sysv",
                 "classic ELF .hash section"),
       clEnumValN(mcld::GeneralOptions::GNU, "gnu",
                 "new style GNU .gnu.hash section"),
       clEnumValN(mcld::GeneralOptions::Both, "both",
                 "both the classic ELF and new style GNU hash tables"),
       clEnumValEnd));

static llvm::cl::opt<std::string>
ArgFilter("F",
          llvm::cl::desc("Filter for shared object symbol table"),
          llvm::cl::value_desc("name"));

static llvm::cl::alias
ArgFilterAlias("filter",
               llvm::cl::desc("alias for -F"),
               llvm::cl::aliasopt(ArgFilterAlias));

static llvm::cl::list<std::string>
ArgAuxiliary("f",
             llvm::cl::ZeroOrMore,
             llvm::cl::desc("Auxiliary filter for shared object symbol table"),
             llvm::cl::value_desc("name"));

static llvm::cl::alias
ArgAuxiliaryAlias("auxiliary",
                  llvm::cl::desc("alias for -f"),
                  llvm::cl::aliasopt(ArgAuxiliary));

static llvm::cl::opt<bool>
ArgUseGold("use-gold",
          llvm::cl::desc("GCC/collect2 compatibility: uses ld.gold.  Ignored"),
          llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgUseMCLD("use-mcld",
          llvm::cl::desc("GCC/collect2 compatibility: uses ld.mcld.  Ignored"),
          llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgUseLD("use-ld",
          llvm::cl::desc("GCC/collect2 compatibility: uses ld.bfd.  Ignored"),
          llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgEB("EB",
      llvm::cl::desc("Link big-endian objects. This affects the default output format."),
      llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgEL("EL",
      llvm::cl::desc("Link little-endian objects. This affects the default output format."),
      llvm::cl::init(false));

static llvm::cl::list<std::string>
ArgPlugin("plugin",
          llvm::cl::desc("Load a plugin library."),
          llvm::cl::value_desc("plugin"));

static llvm::cl::list<std::string>
ArgPluginOpt("plugin-opt",
             llvm::cl::desc("	Pass an option to the plugin."),
             llvm::cl::value_desc("option"));

static llvm::cl::opt<bool>
ArgSVR4Compatibility("Qy",
                    llvm::cl::desc("This option is ignored for SVR4 compatibility"),
                    llvm::cl::init(false));

static llvm::cl::list<std::string>
ArgY("Y",
     llvm::cl::desc("Add path to the default library search path"),
     llvm::cl::value_desc("default-search-path"));

/// @{
/// @name FIXME: end of unsupported options
/// @}/
static llvm::cl::opt<bool>
ArgNoStdlib("nostdlib",
            llvm::cl::desc("Only search lib dirs explicitly specified on cmdline"),
            llvm::cl::init(false));

static llvm::cl::list<std::string, bool, llvm::cl::SearchDirParser>
ArgRuntimePath("rpath",
               llvm::cl::ZeroOrMore,
               llvm::cl::desc("Add a directory to the runtime library search path"),
               llvm::cl::value_desc("dir"));

static llvm::cl::alias
ArgRuntimePathAlias("R",
                    llvm::cl::desc("alias for --rpath"),
                    llvm::cl::aliasopt(ArgRuntimePath), llvm::cl::Prefix);

static llvm::cl::opt<bool>
ArgEnableNewDTags("enable-new-dtags",
                  llvm::cl::desc("Enable use of DT_RUNPATH and DT_FLAGS"),
                  llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgPrintMap("M",
            llvm::cl::desc("Print a link map to the standard output."),
            llvm::cl::init(false));

static llvm::cl::alias
ArgPrintMapAlias("print-map",
                 llvm::cl::desc("alias for -M"),
                 llvm::cl::aliasopt(ArgPrintMap));

static bool ArgFatalWarnings;

static llvm::cl::opt<bool, true, llvm::cl::FalseParser>
ArgNoFatalWarnings("no-fatal-warnings",
              llvm::cl::location(ArgFatalWarnings),
              llvm::cl::desc("do not turn warnings into errors"),
              llvm::cl::init(false),
              llvm::cl::ValueDisallowed);

static llvm::cl::opt<bool, true>
ArgFatalWarningsFlag("fatal-warnings",
              llvm::cl::location(ArgFatalWarnings),
              llvm::cl::desc("turn all warnings into errors"),
              llvm::cl::init(false),
              llvm::cl::ValueDisallowed);

static llvm::cl::opt<bool>
ArgWarnSharedTextrel("warn-shared-textrel",
                     llvm::cl::desc("Warn if adding DT_TEXTREL in a shared object."),
                     llvm::cl::init(false));

namespace format {
enum Format {
  Binary,
  Unknown // decided by triple
};
} // namespace of format

static llvm::cl::opt<format::Format>
ArgFormat("b",
  llvm::cl::value_desc("Format"),
  llvm::cl::desc("set input format"),
  llvm::cl::init(format::Unknown),
  llvm::cl::values(
    clEnumValN(format::Binary, "binary",
      "read in binary machine code."),
    clEnumValEnd));

static llvm::cl::alias
ArgFormatAlias("format",
               llvm::cl::desc("alias for -b"),
               llvm::cl::aliasopt(ArgFormat));

static llvm::cl::opt<format::Format>
ArgOFormat("oformat",
  llvm::cl::value_desc("Format"),
  llvm::cl::desc("set output format"),
  llvm::cl::init(format::Unknown),
  llvm::cl::values(
    clEnumValN(format::Binary, "binary",
      "generate binary machine code."),
    clEnumValEnd));

static llvm::cl::opt<bool>
ArgDefineCommon("d",
                llvm::cl::ZeroOrMore,
                llvm::cl::desc("Define common symbol"),
                llvm::cl::init(false));

static llvm::cl::alias
ArgDefineCommonAlias1("dc",
                      llvm::cl::ZeroOrMore,
                      llvm::cl::desc("alias for -d"),
                      llvm::cl::aliasopt(ArgDefineCommon));

static llvm::cl::alias
ArgDefineCommonAlias2("dp",
                      llvm::cl::ZeroOrMore,
                      llvm::cl::desc("alias for -d"),
                      llvm::cl::aliasopt(ArgDefineCommon));

//===----------------------------------------------------------------------===//
// Scripting Options
//===----------------------------------------------------------------------===//
static llvm::cl::list<std::string>
ArgWrapList("wrap",
            llvm::cl::ZeroOrMore,
            llvm::cl::desc("Use a wrap function fo symbol."),
            llvm::cl::value_desc("symbol"));

static llvm::cl::list<std::string>
ArgPortList("portable",
            llvm::cl::ZeroOrMore,
            llvm::cl::desc("Use a portable function fo symbol."),
            llvm::cl::value_desc("symbol"));

static llvm::cl::list<std::string>
ArgAddressMapList("section-start",
                  llvm::cl::ZeroOrMore,
                  llvm::cl::desc("Locate a output section at the given absolute address"),
                  llvm::cl::value_desc("Set address of section"),
                  llvm::cl::Prefix);

static llvm::cl::list<std::string>
ArgDefSymList("defsym",
              llvm::cl::ZeroOrMore,
              llvm::cl::desc("Define a symbol"),
              llvm::cl::value_desc("symbol=expression"));

static llvm::cl::opt<unsigned long long>
ArgBssSegAddr("Tbss",
              llvm::cl::desc("Set the address of the bss segment"),
              llvm::cl::init(-1U));

static llvm::cl::opt<unsigned long long>
ArgDataSegAddr("Tdata",
               llvm::cl::desc("Set the address of the data segment"),
               llvm::cl::init(-1U));

static llvm::cl::opt<unsigned long long>
ArgTextSegAddr("Ttext",
               llvm::cl::desc("Set the address of the text segment"),
               llvm::cl::init(-1U));

static llvm::cl::alias
ArgTextSegAddrAlias("Ttext-segment",
                    llvm::cl::desc("alias for -Ttext"),
                    llvm::cl::aliasopt(ArgTextSegAddr));

//===----------------------------------------------------------------------===//
// Non-member functions
//===----------------------------------------------------------------------===//
/// ParseProgName - Parse program name
/// This function simplifies cross-compiling by reading triple from the program
/// name. For example, if the program name is `arm-linux-eabi-ld.mcld', we can
/// get the triple is arm-linux-eabi by the program name.
static std::string ParseProgName(const char *progname)
{
  static const char *suffixes[] = {
    "ld",
    "ld.mcld",
  };

  std::string ProgName(mcld::sys::fs::Path(progname).stem().native());

  for (size_t i = 0; i < sizeof(suffixes) / sizeof(suffixes[0]); ++i) {
    if (ProgName == suffixes[i])
      return std::string();
  }

  llvm::StringRef ProgNameRef(ProgName);
  llvm::StringRef Prefix;

  for (size_t i = 0; i < sizeof(suffixes) / sizeof(suffixes[0]); ++i) {
    if (!ProgNameRef.endswith(suffixes[i]))
      continue;

    llvm::StringRef::size_type LastComponent = ProgNameRef.rfind('-',
      ProgNameRef.size() - strlen(suffixes[i]));
    if (LastComponent == llvm::StringRef::npos)
      continue;
    llvm::StringRef Prefix = ProgNameRef.slice(0, LastComponent);
    std::string IgnoredError;
    if (!llvm::TargetRegistry::lookupTarget(Prefix, IgnoredError))
      continue;
    return Prefix.str();
  }
  return std::string();
}

static Triple ParseEmulation(const std::string& pEmulation)
{
  Triple result = StringSwitch<Triple>(pEmulation)
    .Case("armelf_linux_eabi", Triple("arm", "", "linux", "gnueabi"))
    .Case("elf_i386",          Triple("i386", "", "", "gnu"))
    .Case("elf_x86_64",        Triple("x86_64", "", "", "gnu"))
    .Case("elf32_x86_64",      Triple("x86_64", "", "", "gnux32"))
    .Case("elf_i386_fbsd",     Triple("i386", "", "freebsd", "gnu"))
    .Case("elf_x86_64_fbsd",   Triple("x86_64", "", "freebsd", "gnu"))
    .Case("elf32ltsmip",       Triple("mipsel", "", "", "gnu"))
    .Default(Triple());

  if (result.getArch()        == Triple::UnknownArch &&
      result.getOS()          == Triple::UnknownOS &&
      result.getEnvironment() == Triple::UnknownEnvironment)
    mcld::error(mcld::diag::err_invalid_emulation) << pEmulation << "\n";

  return result;
}

static bool ShouldColorize()
{
   const char* term = getenv("TERM");
   return term && (0 != strcmp(term, "dumb"));
}

static bool ProcessLinkerOptionsFromCommand(mcld::LinkerScript& pScript,
                                            mcld::LinkerConfig& pConfig)
{
  // -----  Set up General Options  ----- //
  // set up colorize
  switch (ArgColor) {
    case color::Never:
      pConfig.options().setColor(false);
    break;
    case color::Always:
      pConfig.options().setColor(true);
    break;
    case color::Auto:
      bool color_option = ShouldColorize() &&
                 llvm::sys::Process::FileDescriptorIsDisplayed(STDOUT_FILENO);
      pConfig.options().setColor(color_option);
    break;
  }

  mcld::outs().setColor(pConfig.options().color());
  mcld::errs().setColor(pConfig.options().color());

  // set up soname
  pConfig.options().setSOName(ArgSOName);

  // add all rpath entries
  cl::list<std::string>::iterator rp;
  cl::list<std::string>::iterator rpEnd = ArgRuntimePath.end();
  for (rp = ArgRuntimePath.begin(); rp != rpEnd; ++rp) {
    pConfig.options().getRpathList().push_back(*rp);
  }

  // --fatal-warnings
  // pConfig.options().setFatalWarnings(ArgFatalWarnings);

  // -shared or -pie
  if (true == ArgShared || true == ArgPIE) {
    ArgFileType = mcld::CGFT_DSOFile;
  }
  else if (true == ArgRelocatable) {
    ArgFileType = mcld::CGFT_PARTIAL;
  }
  else if (format::Binary == ArgOFormat) {
    ArgFileType = mcld::CGFT_BINARY;
  }

  // -b [input-format], --format=[input-format]
  if (format::Binary == ArgFormat)
    pConfig.options().setBinaryInput();

  // -V
  if (ArgVersion) {
    mcld::outs() << "MCLinker - "
                 << mcld::LinkerConfig::version()
                 << "\n";
  }

  // set up sysroot
  if (!ArgSysRoot.empty()) {
    if (exists(ArgSysRoot) && is_directory(ArgSysRoot))
      pScript.setSysroot(ArgSysRoot);
  }

  // add all search directories
  cl::list<std::string>::iterator sd;
  cl::list<std::string>::iterator sdEnd = ArgSearchDirList.end();
  for (sd=ArgSearchDirList.begin(); sd!=sdEnd; ++sd) {
    if (!pScript.directories().insert(*sd)) {
      // FIXME: need a warning function
      errs() << "WARNING: can not open search directory `-L"
             << *sd
             << "'.\n";
    }
  }

  pConfig.options().setPIE(ArgPIE);
  pConfig.options().setTrace(ArgTrace);
  pConfig.options().setVerbose(ArgVerbose);
  pConfig.options().setMaxErrorNum(ArgMaxErrorNum);
  pConfig.options().setMaxWarnNum(ArgMaxWarnNum);
  pConfig.options().setBsymbolic(ArgBsymbolic);
  pConfig.options().setBgroup(ArgBgroup);
  pConfig.options().setDyld(ArgDyld);
  pConfig.options().setNoUndefined(ArgNoUndefined);
  pConfig.options().setMulDefs(ArgAllowMulDefs);
  pConfig.options().setEhFrameHdr(ArgEhFrameHdr);
  pConfig.options().setNMagic(ArgNMagic);
  pConfig.options().setOMagic(ArgOMagic);
  pConfig.options().setStripDebug(ArgStripDebug || ArgStripAll);
  pConfig.options().setExportDynamic(ArgExportDynamic);
  pConfig.options().setWarnSharedTextrel(ArgWarnSharedTextrel);
  pConfig.options().setDefineCommon(ArgDefineCommon);
  pConfig.options().setNewDTags(ArgEnableNewDTags);
  pConfig.options().setHashStyle(ArgHashStyle);
  pConfig.options().setNoStdlib(ArgNoStdlib);
  pConfig.options().setPrintMap(ArgPrintMap);
  pConfig.options().setGPSize(ArgGPSize);

  if (ArgStripAll)
    pConfig.options().setStripSymbols(mcld::GeneralOptions::StripAllSymbols);
  else if (ArgDiscardAll)
    pConfig.options().setStripSymbols(mcld::GeneralOptions::StripLocals);
  else if (ArgDiscardLocals)
    pConfig.options().setStripSymbols(mcld::GeneralOptions::StripTemporaries);
  else
    pConfig.options().setStripSymbols(mcld::GeneralOptions::KeepAllSymbols);

  // set up entry point from -e
  pScript.setEntry(ArgEntry);

  // set up rename map, for --wrap
  cl::list<std::string>::iterator wname;
  cl::list<std::string>::iterator wnameEnd = ArgWrapList.end();
  for (wname = ArgWrapList.begin(); wname != wnameEnd; ++wname) {
    bool exist = false;

    // add wname -> __wrap_wname
    mcld::StringEntry<llvm::StringRef>* to_wrap =
                                     pScript.renameMap().insert(*wname, exist);

    std::string to_wrap_str = "__wrap_" + *wname;
    to_wrap->setValue(to_wrap_str);

    if (exist)
      mcld::warning(mcld::diag::rewrap) << *wname << to_wrap_str;

    // add __real_wname -> wname
    std::string from_real_str = "__real_" + *wname;
    mcld::StringEntry<llvm::StringRef>* from_real =
                              pScript.renameMap().insert(from_real_str, exist);
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
                                     pScript.renameMap().insert(*pname, exist);

    std::string to_port_str = *pname + "_portable";
    to_port->setValue(to_port_str);

    if (exist)
      mcld::warning(mcld::diag::rewrap) << *pname << to_port_str;

    // add __real_pname -> pname
    std::string from_real_str = "__real_" + *pname;
    mcld::StringEntry<llvm::StringRef>* from_real =
                              pScript.renameMap().insert(from_real_str, exist);

    from_real->setValue(*pname);
    if (exist)
      mcld::warning(mcld::diag::rewrap) << *pname << from_real_str;
  } // end of for

  // add -z options
  cl::list<mcld::ZOption>::iterator zOpt;
  cl::list<mcld::ZOption>::iterator zOptEnd = ArgZOptionList.end();
  for (zOpt = ArgZOptionList.begin(); zOpt != zOptEnd; ++zOpt) {
    pConfig.options().addZOption(*zOpt);
  }

  if (ArgGCSections) {
    mcld::warning(mcld::diag::warn_unsupported_option) << ArgGCSections.ArgStr;
  }

  // set up icf mode
  switch (ArgICF) {
    case icf::None:
      break;
    case icf::All:
    case icf::Safe:
    default:
      mcld::warning(mcld::diag::warn_unsupported_option) << ArgICF.ArgStr;
      break;
  }

  if (ArgFIXCA8) {
    mcld::warning(mcld::diag::warn_unsupported_option) << ArgFIXCA8.ArgStr;
  }

  // add address mappings
  // -Ttext
  if (-1U != ArgTextSegAddr) {
    bool exist = false;
    mcld::StringEntry<uint64_t>* text_mapping =
                                   pScript.addressMap().insert(".text", exist);
    text_mapping->setValue(ArgTextSegAddr);
  }
  // -Tdata
  if (-1U != ArgDataSegAddr) {
    bool exist = false;
    mcld::StringEntry<uint64_t>* data_mapping =
                                   pScript.addressMap().insert(".data", exist);
    data_mapping->setValue(ArgDataSegAddr);
  }
  // -Tbss
  if (-1U != ArgBssSegAddr) {
    bool exist = false;
    mcld::StringEntry<uint64_t>* bss_mapping =
                                    pScript.addressMap().insert(".bss", exist);
    bss_mapping->setValue(ArgBssSegAddr);
  }
  // --section-start SECTION=ADDRESS
  for (cl::list<std::string>::iterator
       it = ArgAddressMapList.begin(), ie = ArgAddressMapList.end();
       it != ie; ++it) {
    // FIXME: Add a cl::parser
    size_t pos = (*it).find_last_of('=');
    llvm::StringRef script(*it);
    uint64_t address = 0x0;
    script.substr(pos + 1).getAsInteger(0, address);
    bool exist = false;
    mcld::StringEntry<uint64_t>* addr_mapping =
                     pScript.addressMap().insert(script.substr(0, pos), exist);
    addr_mapping->setValue(address);
  }

  // --defsym symbols
  for (cl::list<std::string>::iterator
       it = ArgDefSymList.begin(), ie = ArgDefSymList.end();
       it != ie ; ++it) {
    pScript.defSyms().append(*it);
  }

  // set up filter/aux filter for shared object
  pConfig.options().setFilter(ArgFilter);

  cl::list<std::string>::iterator aux;
  cl::list<std::string>::iterator auxEnd = ArgAuxiliary.end();
  for (aux = ArgAuxiliary.begin(); aux != auxEnd; ++aux)
    pConfig.options().getAuxiliaryList().push_back(*aux);

  return true;
}

int main(int argc, char* argv[])
{
}
