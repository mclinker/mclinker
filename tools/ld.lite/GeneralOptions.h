//===- GeneralOptions.h ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDLITE_GENERAL_OPTIONS_H
#define MCLD_LDLITE_GENERAL_OPTIONS_H

//===----------------------------------------------------------------------===//
// General Options
// There are eight kinds of general options:
//   1. Target selection option. Used to select target and define triple.
//   2. Output filename and foramt. Used to define and change the output file
//      name, type and format. For example, options to add special sections
//      belongs this group.
//   3. Dynamic section related option. Used to define and change the tags of
//      dynamic sections.
//   4. Search path and directory. Used to define the search patch and
//      directory.
//   5. Symbols and reference. This kind of options are used to define and
//      modify the symbols and references. Change to topology of fragment-
//      reference graph.
//   6. Target specific options. Some targets require special options. This
//      group is used to collect such kind of options.
//   7. Optimization options. Used to enable optimizations.
//   8. Linker preference. Used to set the preference of linker. These options
//      change the behavior of linking process.
//===----------------------------------------------------------------------===//
// Target Selection Options
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

static llvm::cl::opt<std::string>
ArgEmulation("m",
             llvm::cl::ZeroOrMore,
             llvm::cl::desc("Set GNU linker emulation"),
             llvm::cl::value_desc("emulation"));

//===----------------------------------------------------------------------===//
// Output Filename and Format
//===----------------------------------------------------------------------===//
static llvm::cl::opt<mcld::sys::fs::Path, false, llvm::cl::parser<mcld::sys::fs::Path> >
ArgOutputFilename("o",
               llvm::cl::desc("Output filename"),
               llvm::cl::value_desc("filename"));

static llvm::cl::alias
AliasOutputFilename("output",
                    llvm::cl::desc("alias for -o"),
                    llvm::cl::aliasopt(ArgOutputFilename));

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

static llvm::cl::opt<mcld::Input::Type>
ArgFormat("b",
  llvm::cl::value_desc("Format"),
  llvm::cl::desc("set input format"),
  llvm::cl::init(mcld::Input::Unknown),
  llvm::cl::values(
    clEnumValN(mcld::Input::Binary, "binary",
      "read in binary machine code."),
    clEnumValEnd));

static llvm::cl::alias
ArgFormatAlias("format",
               llvm::cl::desc("alias for -b"),
               llvm::cl::aliasopt(ArgFormat));

static llvm::cl::opt<mcld::LinkerConfig::CodeGenType>
ArgOFormat("oformat",
  llvm::cl::value_desc("Format"),
  llvm::cl::desc("set output format"),
  llvm::cl::init(mcld::LinkerConfig::Unknown),
  llvm::cl::values(
    clEnumValN(mcld::LinkerConfig::Binary, "binary",
      "generate binary machine code."),
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

static llvm::cl::opt<bool>
ArgStripDebug("strip-debug",
              llvm::cl::desc("Omit debugger symbol information from the output file."),
              llvm::cl::init(false));

static llvm::cl::alias
ArgStripDebugAlias("S",
                   llvm::cl::desc("alias for --strip-debug"),
                   llvm::cl::aliasopt(ArgStripDebug));

static llvm::cl::opt<bool>
ArgEhFrameHdr("eh-frame-hdr",
              llvm::cl::desc("Request creation of \".eh_frame_hdr\" section and ELF \"PT_GNU_EH_FRAME\" segment header."),
              llvm::cl::init(false));

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

// Not supported yet {
static llvm::cl::opt<bool>
ArgExportDynamic("export-dynamic",
                 llvm::cl::desc("Export all dynamic symbols"),
                 llvm::cl::init(false));

static llvm::cl::alias
ArgExportDynamicAlias("E",
                      llvm::cl::desc("alias for --export-dynamic"),
                      llvm::cl::aliasopt(ArgExportDynamic));

static llvm::cl::opt<std::string>
ArgBuildID("build-id",
           llvm::cl::desc("Request creation of \".note.gnu.build-id\" ELF note section."),
           llvm::cl::value_desc("style"),
           llvm::cl::ValueOptional);

static llvm::cl::list<std::string>
ArgExcludeLIBS("exclude-libs",
               llvm::cl::CommaSeparated,
               llvm::cl::desc("Exclude libraries from automatic export"),
               llvm::cl::value_desc("lib1,lib2,..."));

// } Not supported yet

//===----------------------------------------------------------------------===//
// Dynamic Section Related
//===----------------------------------------------------------------------===//
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

static llvm::cl::list<mcld::ZOption, bool, llvm::cl::parser<mcld::ZOption> >
ArgZOptionList("z",
               llvm::cl::ZeroOrMore,
               llvm::cl::desc("The -z options for GNU ld compatibility."),
               llvm::cl::value_desc("keyword"),
               llvm::cl::Prefix);

static llvm::cl::opt<std::string>
ArgDyld("dynamic-linker",
        llvm::cl::ZeroOrMore,
        llvm::cl::desc("Set the name of the dynamic linker."),
        llvm::cl::value_desc("Program"));

static llvm::cl::opt<bool>
ArgEnableNewDTags("enable-new-dtags",
                  llvm::cl::desc("Enable use of DT_RUNPATH and DT_FLAGS"),
                  llvm::cl::init(false));

// Not supported yet {
static llvm::cl::list<std::string>
ArgAuxiliary("f",
             llvm::cl::ZeroOrMore,
             llvm::cl::desc("Auxiliary filter for shared object symbol table"),
             llvm::cl::value_desc("name"));

static llvm::cl::alias
ArgAuxiliaryAlias("auxiliary",
                  llvm::cl::desc("alias for -f"),
                  llvm::cl::aliasopt(ArgAuxiliary));

static llvm::cl::opt<std::string>
ArgFilter("F",
          llvm::cl::desc("Filter for shared object symbol table"),
          llvm::cl::value_desc("name"));

static llvm::cl::alias
ArgFilterAlias("filter",
               llvm::cl::desc("alias for -F"),
               llvm::cl::aliasopt(ArgFilterAlias));

// } Not supported yet

//===----------------------------------------------------------------------===//
// Search Path and Directory
//===----------------------------------------------------------------------===//
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

// Not supported yet {
static llvm::cl::list<std::string, bool, llvm::cl::SearchDirParser>
ArgRuntimePathLink("rpath-link",
                   llvm::cl::ZeroOrMore,
                   llvm::cl::desc("Add a directory to the link time library search path"),
                   llvm::cl::value_desc("dir"));

static llvm::cl::list<std::string>
ArgY("Y",
     llvm::cl::desc("Add path to the default library search path"),
     llvm::cl::value_desc("default-search-path"));

// } Not supported yet

//===----------------------------------------------------------------------===//
// Symbols and References Operations
//===----------------------------------------------------------------------===//
static llvm::cl::opt<bool>
ArgNoUndefined("no-undefined",
               llvm::cl::desc("Do not allow unresolved references"),
               llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgAllowMulDefs("allow-multiple-definition",
                llvm::cl::desc("Allow multiple definition"),
                llvm::cl::init(false));

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
ArgStripAll("strip-all",
            llvm::cl::desc("Omit all symbol information from the output file."),
            llvm::cl::init(false));

static llvm::cl::alias
ArgStripAllAlias("s",
                 llvm::cl::desc("alias for --strip-all"),
                 llvm::cl::aliasopt(ArgStripAll));

// Not supprted yet {
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

// } Not supported yet

//===----------------------------------------------------------------------===//
// Target Specific
//===----------------------------------------------------------------------===//
static llvm::cl::opt<int>
ArgGPSize("G",
          llvm::cl::desc("Set the maximum size of objects to be optimized using GP"),
          llvm::cl::init(8));

static llvm::cl::opt<bool>
ArgWarnSharedTextrel("warn-shared-textrel",
                     llvm::cl::desc("Warn if adding DT_TEXTREL in a shared object."),
                     llvm::cl::init(false));

// Not supported yet {
// FIXME: add this to target options?
static llvm::cl::opt<bool>
ArgFIXCA8("fix-cortex-a8",
          llvm::cl::desc("Enable Cortex-A8 Thumb-2 branch erratum fix"),
          llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgEB("EB",
      llvm::cl::desc("Link big-endian objects. This affects the default output format."),
      llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgEL("EL",
      llvm::cl::desc("Link little-endian objects. This affects the default output format."),
      llvm::cl::init(false));

static llvm::cl::opt<bool>
ArgSVR4Compatibility("Qy",
                    llvm::cl::desc("This option is ignored for SVR4 compatibility"),
                    llvm::cl::init(false));

// } Not supported yet

//===----------------------------------------------------------------------===//
// Optimization
//===----------------------------------------------------------------------===//
// Not supported yet {
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

static llvm::cl::list<std::string>
ArgPlugin("plugin",
          llvm::cl::desc("Load a plugin library."),
          llvm::cl::value_desc("plugin"));

static llvm::cl::list<std::string>
ArgPluginOpt("plugin-opt",
             llvm::cl::desc("	Pass an option to the plugin."),
             llvm::cl::value_desc("option"));

// } Not supported yet

//===----------------------------------------------------------------------===//
// Linker Preference and Misc.
//===----------------------------------------------------------------------===//
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

#endif

