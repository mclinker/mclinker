//===- OptimizationOptions.cpp --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/OptimizationOptions.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Support/CommandLine.h>
#include <mcld/Support/MsgHandling.h>

namespace {

bool ArgGCSections;

llvm::cl::opt<bool, true> ArgGCSectionsFlag("gc-sections",
  llvm::cl::ZeroOrMore,
  llvm::cl::location(ArgGCSections),
  llvm::cl::desc("Enable garbage collection of unused input sections."),
  llvm::cl::init(false));

llvm::cl::opt<bool, true, llvm::cl::FalseParser> ArgNoGCSectionsFlag("no-gc-sections",
  llvm::cl::ZeroOrMore,
  llvm::cl::location(ArgGCSections),
  llvm::cl::desc("disable garbage collection of unused input sections."),
  llvm::cl::init(false));

bool ArgPrintGCSections;

llvm::cl::opt<bool, true> ArgPrintGCSectionsFlag("print-gc-sections",
  llvm::cl::ZeroOrMore,
  llvm::cl::location(ArgPrintGCSections),
  llvm::cl::desc("List all sections removed by garbage collection."),
  llvm::cl::init(false));

llvm::cl::opt<bool, true, llvm::cl::FalseParser> ArgNoPrintGCSectionsFlag("no-print-gc-sections",
  llvm::cl::ZeroOrMore,
  llvm::cl::location(ArgPrintGCSections),
  llvm::cl::desc("disable --print-gc-sections"),
  llvm::cl::init(false));

bool ArgGenUnwindInfo;

llvm::cl::opt<bool, true, llvm::cl::FalseParser>
ArgNoGenUnwindInfoFlag("no-ld-generated-unwind-info",
                       llvm::cl::ZeroOrMore,
                       llvm::cl::location(ArgGenUnwindInfo),
                       llvm::cl::desc("Don't create unwind info for linker"
                                      " generated sections to save size"),
                       llvm::cl::init(false),
                       llvm::cl::ValueDisallowed);
llvm::cl::opt<bool, true>
ArgGenUnwindInfoFlag("ld-generated-unwind-info",
                     llvm::cl::ZeroOrMore,
                     llvm::cl::location(ArgGenUnwindInfo),
                     llvm::cl::desc("Request creation of unwind info for linker"
                                    " generated code sections like PLT."),
                     llvm::cl::init(true),
                     llvm::cl::ValueDisallowed);

llvm::cl::opt<mcld::GeneralOptions::ICF> ArgICF("icf",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Identical Code Folding"),
  llvm::cl::init(mcld::GeneralOptions::ICF_None),
  llvm::cl::values(
    clEnumValN(mcld::GeneralOptions::ICF_None, "none",
      "do not perform cold folding"),
    clEnumValN(mcld::GeneralOptions::ICF_All, "all",
      "always preform cold folding"),
    clEnumValN(mcld::GeneralOptions::ICF_Safe, "safe",
      "Folds those whose pointers are definitely not taken."),
    clEnumValEnd));

llvm::cl::opt<unsigned> ArgICFIterations("icf-iterations",
  llvm::cl::desc("Number of iterations to do ICF."),
  llvm::cl::init(2));

llvm::cl::opt<bool> ArgPrintICFSections("print-icf-sections",
  llvm::cl::desc("Print the folded identical sections."),
  llvm::cl::init(false));

llvm::cl::opt<char> ArgOptLevel("O",
  llvm::cl::desc("Optimization level. [-O0, -O1, -O2, or -O3] "
                 "(default = '-O2')"),
  llvm::cl::Prefix,
  llvm::cl::ZeroOrMore,
  llvm::cl::init(' '));

llvm::cl::list<std::string> ArgPlugin("plugin",
  llvm::cl::desc("Load a plugin library."),
  llvm::cl::value_desc("plugin"));

llvm::cl::list<std::string> ArgPluginOpt("plugin-opt",
  llvm::cl::desc("Pass an option to the plugin."),
  llvm::cl::value_desc("option"));

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// OptimizationOptions
//===----------------------------------------------------------------------===//
OptimizationOptions::OptimizationOptions()
  : m_GCSections(ArgGCSections),
    m_PrintGCSections(ArgPrintGCSections),
    m_GenUnwindInfo(ArgGenUnwindInfo),
    m_ICF(ArgICF),
    m_ICFIterations(ArgICFIterations),
    m_PrintICFSections(ArgPrintICFSections),
    m_OptLevel(ArgOptLevel),
    m_Plugin(ArgPlugin),
    m_PluginOpt(ArgPluginOpt) {
}

bool OptimizationOptions::parse(LinkerConfig& pConfig)
{
  // set --gc-sections
  if (m_GCSections)
    pConfig.options().setGCSections();

  // set --print-gc-sections
  if (m_PrintGCSections)
    pConfig.options().setPrintGCSections();

  // set --ld-generated-unwind-info (or not)
  pConfig.options().setGenUnwindInfo(m_GenUnwindInfo);

  // set --icf [mode]
  pConfig.options().setICFMode(m_ICF);
  pConfig.options().setICFIterations(m_ICFIterations);
  pConfig.options().setPrintICFSections(m_PrintICFSections);

  return true;
}
