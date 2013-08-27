//===- ld-lite.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <lite/TripleOptions.h>
#include <lite/OutputFormatOptions.h>

#include <mcld/Module.h>
#include <mcld/Environment.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/Linker.h>
#include <mcld/IRBuilder.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/Support/MsgHandling.h>
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

/// configure linker
static inline bool ConfigLinker(int pArgc,
                                char* pArgv[],
                                const char* pName,
                                mcld::LinkerScript& pScript,
                                mcld::LinkerConfig& pConfig,
                                std::string& pError)
{
  mcld::TripleOptions triple_options;
  mcld::OutputFormatOptions output_format_options;
  
  llvm::cl::ParseCommandLineOptions(pArgc, pArgv, pName);

  if (!triple_options.parse(pArgc, pArgv, pConfig))
    return false;

  if (!output_format_options.parse(pArgc, pArgv, pConfig))
    return false;
  return true;
}

int main(int argc, char* argv[])
{
  mcld::Initialize();

  mcld::LinkerScript script;
  mcld::LinkerConfig config;
  std::string error;
  if (!ConfigLinker(argc, argv, "MCLinker\n", script, config, error)) {
    mcld::errs() << argv[0]
                 << ": failed to process linker options from command line!\n"
                 << "(Detail: " << error << ")\n";
    return 1;
  }

  mcld::Module module(script);
  mcld::IRBuilder builder(module, config);
  mcld::Linker linker;
  mcld::Finalize();
}
