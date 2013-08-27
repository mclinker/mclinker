//===- ld-lite.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <lite/PreferenceOptions.h>
#include <lite/TripleOptions.h>
#include <lite/DynamicSectionOptions.h>
#include <lite/OutputFormatOptions.h>
#include <lite/SearchPathOptions.h>
#include <lite/OptimizationOptions.h>
#include <lite/SymbolOptions.h>
#include <lite/TargetControlOptions.h>
#include <lite/ScriptOptions.h>
#include <lite/PositionalOptions.h>

#include <mcld/Module.h>
#include <mcld/Environment.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/Linker.h>
#include <mcld/IRBuilder.h>
#include <mcld/Support/raw_ostream.h>
#include <string>

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
