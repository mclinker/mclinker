//===- TripleOptions.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/TripleOptions.h>

#include <mcld/LinkerConfig.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/SystemUtils.h>

#include <llvm/ADT/StringSwitch.h>
#include <llvm/MC/SubtargetFeature.h>

namespace {

llvm::cl::opt<std::string> ArgTargetTriple("mtriple",
                           llvm::cl::desc("Override target triple for module"));

llvm::cl::opt<std::string> ArgMArch("march",
           llvm::cl::desc("Architecture to generate code for (see --version)"));

llvm::cl::opt<std::string> ArgMCPU("mcpu",
          llvm::cl::desc("Target a specific cpu type (-mcpu=help for details)"),
          llvm::cl::value_desc("cpu-name"),
          llvm::cl::init(""));

llvm::cl::list<std::string> ArgMAttrs("mattr",
         llvm::cl::CommaSeparated,
         llvm::cl::desc("Target specific attributes (-mattr=help for details)"),
         llvm::cl::value_desc("a1,+a2,-a3,..."));

llvm::cl::opt<std::string> ArgEmulation("m",
                                     llvm::cl::ZeroOrMore,
                                     llvm::cl::desc("Set GNU linker emulation"),
                                     llvm::cl::value_desc("emulation"),
                                     llvm::cl::Prefix);

/// ParseProgName - Parse program name
/// This function simplifies cross-compiling by reading triple from the program
/// name. For example, if the program name is `arm-linux-eabi-ld.mcld', we can
/// get the triple is arm-linux-eabi by the program name.
inline std::string ParseProgName(const char *pProgName)
{
  static const char *suffixes[] = {
    "ld",
    "ld.mcld"
  };

  std::string ProgName(mcld::sys::fs::Path(pProgName).stem().native());

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

inline void
ParseEmulation(llvm::Triple& pTriple, const std::string& pEmulation)
{
  llvm::Triple triple = llvm::StringSwitch<llvm::Triple>(pEmulation)
    .Case("aarch64linux",      llvm::Triple("aarch64", "", "linux", "gnu"))
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

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// TripleOptions
//===----------------------------------------------------------------------===//
TripleOptions::TripleOptions()
  : m_TargetTriple(ArgTargetTriple),
    m_MArch(ArgMArch),
    m_MCPU(ArgMCPU),
    m_MAttrs(ArgMAttrs),
    m_Emulation(ArgEmulation) {
}

bool TripleOptions::parse(int pArgc, char* pArgv[], LinkerConfig& pConfig)
{
  llvm::Triple triple;
  if (!m_TargetTriple.empty()) {
    // 1. Use the triple from command.
    triple.setTriple(m_TargetTriple);
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
  if (!m_Emulation.empty())
    ParseEmulation(triple, m_Emulation);
  else
    pConfig.targets().setArch(m_MArch);

  pConfig.targets().setTriple(triple);
  pConfig.targets().setTargetCPU(m_MCPU);

  // Package up features to be passed to target/subtarget
  std::string feature_str;
  if (m_MAttrs.size()) {
    llvm::SubtargetFeatures features;
    for (unsigned i = 0; i != m_MAttrs.size(); ++i)
      features.AddFeature(m_MAttrs[i]);
    feature_str = features.getString();
  }
  pConfig.targets().setTargetFeatureString(feature_str);
  return true;
}

