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
#include <mcld/MC/InputAction.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/Support/MsgHandling.h>
#include <llvm/Support/ManagedStatic.h>
#include <llvm/Support/Signals.h>
#include <string>

/// configure linker
static inline bool ConfigLinker(int pArgc,
                                char* pArgv[],
                                const char* pName,
                                mcld::Module& pModule,
                                mcld::LinkerScript& pScript,
                                mcld::LinkerConfig& pConfig,
                                mcld::IRBuilder& pBuilder)
{
  mcld::PreferenceOptions     preference;
  mcld::TripleOptions         triple;
  mcld::DynamicSectionOptions dynamic_section;
  mcld::OutputFormatOptions   output_format;
  mcld::SearchPathOptions     search_path;
  mcld::OptimizationOptions   optimization;
  mcld::SymbolOptions         symbol;
  mcld::TargetControlOptions  target_control;
  mcld::ScriptOptions         script;
  mcld::PositionalOptions     positional;
  
  llvm::cl::ParseCommandLineOptions(pArgc, pArgv, pName);

  if (!preference.parse(pConfig))
    return false;

  if (!triple.parse(pArgc, pArgv, pConfig))
    return false;

  if (!dynamic_section.parse(pConfig, pScript))
    return false;

  if (!output_format.parse(pModule, pConfig))
    return false;

  if (!search_path.parse(pConfig, pScript))
    return false;

  if (!optimization.parse(pConfig))
    return false;

  if (!symbol.parse(pConfig))
    return false;

  if (!target_control.parse(pConfig))
    return false;

  if (!script.parse(pScript))
    return false;

  typedef std::vector<mcld::InputAction*> ActionList;

  ActionList actions;
  if (!positional.parse(actions, pConfig, pScript))
    return false;

  ActionList::iterator action, actionEnd = actions.end();
  for (action = actions.begin(); action != actionEnd; ++action) {
    (*action)->activate(pBuilder.getInputBuilder());
    delete *action;
  }

  if (pBuilder.getInputBuilder().isInGroup()) {
    mcld::fatal(mcld::diag::fatal_forbid_nest_group);
    return false;
  }
  return true;
}

int main(int argc, char* argv[])
{
  llvm::sys::PrintStackTraceOnErrorSignal();
  llvm::llvm_shutdown_obj Y;  // Call llvm_shutdown() on exit.
  mcld::Initialize();

  mcld::LinkerScript script;
  mcld::LinkerConfig config;
  mcld::Module module(script);
  mcld::IRBuilder builder(module, config);


  if (!ConfigLinker(argc, argv, "MCLinker\n", module, script, config, builder)) {
    mcld::errs() << argv[0]
                 << ": failed to process linker options from command line!\n";
    return 1;
  }

  mcld::Linker linker;
  if (!linker.emulate(script, config)) {
    mcld::errs() << argv[0]
                 << ": failed to emulate target!\n";
  }

  if (!linker.link(module, builder)) {
    mcld::errs() << argv[0]
                 << ": failed to link objects!\n";
    return 1;
  }

  if (!linker.emit(module.name())) {
    mcld::errs() << argv[0]
                 << ": failed to emit output!\n";
    return 1;
  }
  mcld::Finalize();
}
