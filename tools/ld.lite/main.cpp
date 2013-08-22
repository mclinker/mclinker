//===- ld-lite.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Module.h>
#include <mcld/Environment.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/SystemUtils.h>
#include <mcld/Support/raw_ostream.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/StringSwitch.h>
#include <llvm/ADT/Triple.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/Process.h>
#include <llvm/MC/SubtargetFeature.h>
#include <string>
#include <cstring>

//===----------------------------------------------------------------------===//
// Command Line Options
// There are four kinds of command line options:
//   1. Attribute options. Attributes describes the input file after them. For
//      example, --as-needed affects the input file after this option. Attribute
//      options are not attributes. Attribute options are the options that is
//      used to define a legal attribute.
//   2. Scripting options, Used to represent a subset of link scripting
//      language, such as --defsym.
//   3. General options. (the rest of options)
//===----------------------------------------------------------------------===//
#include "PositionalOptions.h"
#include "ScriptOptions.h"
#include "GeneralOptions.h"

//===----------------------------------------------------------------------===//
// Non-member functions
//===----------------------------------------------------------------------===//
static bool ShouldColorize()
{
   const char* term = getenv("TERM");
   return term && (0 != strcmp(term, "dumb"));
}

static inline void
SetUpEnvironment(mcld::LinkerConfig& pConfig)
{
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
}

/// ParseProgName - Parse program name
/// This function simplifies cross-compiling by reading triple from the program
/// name. For example, if the program name is `arm-linux-eabi-ld.mcld', we can
/// get the triple is arm-linux-eabi by the program name.
static std::string ParseProgName(const char *progname)
{
  static const char *suffixes[] = {
    "ld",
    "ld.mcld",
    "ld.lite",
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
    if (!mcld::TargetRegistry::lookupTarget(Prefix, IgnoredError))
      continue;
    return Prefix.str();
  }
  return std::string();
}

static inline void
ParseEmulation(llvm::Triple& pTriple, const std::string& pEmulation)
{
  llvm::Triple triple = llvm::StringSwitch<llvm::Triple>(pEmulation)
    .Case("armelf_linux_eabi", llvm::Triple("arm", "", "linux", "gnueabi"))
    .Case("elf_i386",          llvm::Triple("i386", "", "", "gnu"))
    .Case("elf_x86_64",        llvm::Triple("x86_64", "", "", "gnu"))
    .Case("elf32_x86_64",      llvm::Triple("x86_64", "", "", "gnux32"))
    .Case("elf_i386_fbsd",     llvm::Triple("i386", "", "freebsd", "gnu"))
    .Case("elf_x86_64_fbsd",   llvm::Triple("x86_64", "", "freebsd", "gnu"))
    .Case("elf32ltsmip",       llvm::Triple("mipsel", "", "", "gnu"))
    .Default(llvm::Triple());

  if (triple.getArch()        == llvm::Triple::UnknownArch &&
      triple.getOS()          == llvm::Triple::UnknownOS &&
      triple.getEnvironment() == llvm::Triple::UnknownEnvironment)
    mcld::error(mcld::diag::err_invalid_emulation) << pEmulation << "\n";

  if (triple.getArch()        != llvm::Triple::UnknownArch)
    pTriple.setArch(triple.getArch());

  if (triple.getOS()          != llvm::Triple::UnknownOS)
    pTriple.setOS(triple.getOS());

  if (triple.getEnvironment() != llvm::Triple::UnknownEnvironment)
    pTriple.setEnvironment(triple.getEnvironment());

}

static inline void
ParseTriple(int pArgc, char* pArgv[], mcld::LinkerConfig& pConfig)
{
  llvm::Triple triple;
  if (!TargetTriple.empty()) {
    // 1. Use the triple from command.
    triple.setTriple(TargetTriple);
  }
  else {
    std::string prog_triple = ParseProgName(pArgv[0]);
    if (!prog_triple.empty()) {
      // 2. Use the triple from the program name prefix.
      triple.setTriple(prog_triple);
    }
    else {
      // 3. Use the default target triple.
      triple.setTriple(mcld::sys::getDefaultTargetTriple());
    }
  }

  // If a specific emulation was requested, apply it now.
  if (!ArgEmulation.empty())
    ParseEmulation(triple, ArgEmulation);

  pConfig.targets().setTriple(triple);
  pConfig.targets().setTargetCPU(MCPU);

  // Package up features to be passed to target/subtarget
  std::string feature_str;
  if (MAttrs.size()) {
    llvm::SubtargetFeatures features;
    for (unsigned i = 0; i != MAttrs.size(); ++i)
      features.AddFeature(MAttrs[i]);
    feature_str = features.getString();
  }
  pConfig.targets().setTargetFeatureString(feature_str);
}

static bool ParseCommandLineOptions(int pArgc,
                                    char* pArgv[],
                                    const char* pName,
                                    mcld::LinkerScript& pScript,
                                    mcld::LinkerConfig& pConfig)
{
  llvm::cl::ParseCommandLineOptions(pArgc, pArgv, pName);

  SetUpEnvironment(pConfig);

  /// Set up general options
  ParseTriple(pArgc, pArgv, pConfig);

  return true;
}

int main(int argc, char* argv[])
{
  mcld::Initialize();

  mcld::LinkerScript script;
  mcld::LinkerConfig config;

  if (!ParseCommandLineOptions(argc, argv, "MCLinker\n", script, config)) {
    mcld::errs() << argv[0]
                 << ": failed to process linker options from command line!\n";
    return 1;
  }
  mcld::Finalize();
}
