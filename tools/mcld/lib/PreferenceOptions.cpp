//===- PreferenceOptions.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/PreferenceOptions.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/Support/raw_ostream.h>
#include <llvm/Support/Process.h>

#if defined(HAVE_UNISTD_H)
# include <unistd.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)
#include <io.h>
#ifndef STDIN_FILENO
# define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
# define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
# define STDERR_FILENO 2
#endif
#endif

namespace {

llvm::cl::opt<bool> ArgTrace("t",
  llvm::cl::desc("Print the names of the input files as ld processes them."));

llvm::cl::alias ArgTraceAlias("trace",
  llvm::cl::desc("alias for -t"),
  llvm::cl::aliasopt(ArgTrace));

llvm::cl::opt<int> ArgVerbose("verbose",
  llvm::cl::init(-1),
  llvm::cl::desc("Display the version number for ld and list the\n"
                 "linker emulations supported."));

llvm::cl::opt<bool> ArgVersion("V",
  llvm::cl::init(false),
  llvm::cl::desc("Display the version number for MCLinker."));

llvm::cl::opt<int> ArgMaxErrorNum("error-limit",
  llvm::cl::init(-1),
  llvm::cl::desc("limits the maximum number of erros."));

llvm::cl::opt<int> ArgMaxWarnNum("warning-limit",
  llvm::cl::init(-1),
  llvm::cl::desc("limits the maximum number of warnings."));

llvm::cl::opt<mcld::PreferenceOptions::Color> ArgColor("color",
  llvm::cl::value_desc("When"),
  llvm::cl::desc("Surround the result strings with the marker"),
  llvm::cl::init(mcld::PreferenceOptions::COLOR_Auto),
  llvm::cl::values(
    clEnumValN(mcld::PreferenceOptions::COLOR_Never, "never",
      "do not surround result"),
    clEnumValN(mcld::PreferenceOptions::COLOR_Always, "always",
      "always surround results, even the output is a plain file"),
    clEnumValN(mcld::PreferenceOptions::COLOR_Auto, "auto",
      "surround result strings only if the output is a tty"),
    clEnumValEnd));

llvm::cl::opt<bool> ArgPrintMap("M",
  llvm::cl::desc("Print a link map to the standard output."),
  llvm::cl::init(false));

llvm::cl::alias ArgPrintMapAlias("print-map",
  llvm::cl::desc("alias for -M"),
  llvm::cl::aliasopt(ArgPrintMap));

bool ArgFatalWarnings;

llvm::cl::opt<bool, true, llvm::cl::FalseParser> ArgNoFatalWarnings("no-fatal-warnings",
  llvm::cl::location(ArgFatalWarnings),
  llvm::cl::desc("do not turn warnings into errors"),
  llvm::cl::init(false),
  llvm::cl::ValueDisallowed);

llvm::cl::opt<bool, true> ArgFatalWarningsFlag("fatal-warnings",
  llvm::cl::ZeroOrMore,
  llvm::cl::location(ArgFatalWarnings),
  llvm::cl::desc("turn all warnings into errors"),
  llvm::cl::init(false),
  llvm::cl::ValueDisallowed);

llvm::cl::opt<std::string> ArgUseLD("fuse-ld",
  llvm::cl::desc("Ignored for GCC/collect2 linker compatibility."),
  llvm::cl::init("mcld"));

llvm::cl::opt<std::string> ArgUseMCLD("use-mcld",
  llvm::cl::desc("Ignored for GCC/collect2 linker compatibility."),
  llvm::cl::init("mcld"));

//===----------------------------------------------------------------------===//
// Non-member functions
//===----------------------------------------------------------------------===//
inline bool ShouldColorize()
{
   const char* term = getenv("TERM");
   return term && (0 != strcmp(term, "dumb"));
}

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// PreferenceOptions
//===----------------------------------------------------------------------===//
PreferenceOptions::PreferenceOptions()
  : m_Trace(ArgTrace),
    m_Verbose(ArgVerbose),
    m_Version(ArgVersion),
    m_MaxErrorNum(ArgMaxErrorNum),
    m_MaxWarnNum(ArgMaxWarnNum),
    m_Color(ArgColor),
    m_PrintMap(ArgPrintMap),
    m_FatalWarnings(ArgFatalWarnings) {
}

bool PreferenceOptions::parse(LinkerConfig& pConfig)
{
  // set -t
  pConfig.options().setTrace(m_Trace);

  // set --verbose
  pConfig.options().setVerbose(m_Verbose);

  // set --error-limit [number]
  pConfig.options().setMaxErrorNum(m_MaxErrorNum);

  // set --warning-limit [number]
  pConfig.options().setMaxWarnNum(m_MaxWarnNum);

  // set --color [mode]
  switch (m_Color) {
    case COLOR_Never:
      pConfig.options().setColor(false);
    break;
    case COLOR_Always:
      pConfig.options().setColor(true);
    break;
    case COLOR_Auto:
      bool color_option = ShouldColorize() &&
                 llvm::sys::Process::FileDescriptorIsDisplayed(STDOUT_FILENO);
      pConfig.options().setColor(color_option);
    break;
  }

  mcld::outs().setColor(pConfig.options().color());
  mcld::errs().setColor(pConfig.options().color());

  return true;
}

