//===- DynamicSectionOptions.cpp ------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/DynamicSectionOptions.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/MC/ZOption.h>

namespace {

llvm::cl::opt<std::string> ArgEntry("e",
  llvm::cl::desc("Use the explicit symbol as the entrance of your program."),
  llvm::cl::value_desc("entry"),
  llvm::cl::ValueRequired);

llvm::cl::alias ArgEntryAlias("entry",
  llvm::cl::desc("alias for -e"),
  llvm::cl::aliasopt(ArgEntry));

llvm::cl::opt<bool> ArgBsymbolic("Bsymbolic",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Bind references within the shared library."),
  llvm::cl::init(false));

llvm::cl::opt<bool> ArgBgroup("Bgroup",
  llvm::cl::desc("Info the dynamic linker to lookups only inside the group."),
  llvm::cl::init(false));

llvm::cl::opt<std::string> ArgSOName("soname",
  llvm::cl::desc("Set internal name of shared library"),
  llvm::cl::value_desc("name"));

llvm::cl::opt<llvm::cl::boolOrDefault> ArgNoUndefined("no-undefined",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Do not allow unresolved references"));

llvm::cl::opt<llvm::cl::boolOrDefault> ArgAllowMulDefs("allow-multiple-definition",
  llvm::cl::desc("Allow multiple definition"));

llvm::cl::list<mcld::ZOption,
               bool,
               llvm::cl::parser<mcld::ZOption> > ArgZOptionList("z",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("The -z options for GNU ld compatibility."),
  llvm::cl::value_desc("keyword"),
  llvm::cl::Prefix);

llvm::cl::opt<std::string> ArgDyld("dynamic-linker",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Set the name of the dynamic linker."),
  llvm::cl::value_desc("Program"));

llvm::cl::opt<bool> ArgEnableNewDTags("enable-new-dtags",
  llvm::cl::desc("Enable use of DT_RUNPATH and DT_FLAGS"),
  llvm::cl::init(false));

// Not supported yet {
llvm::cl::list<std::string> ArgAuxiliary("f",
  llvm::cl::ZeroOrMore,
  llvm::cl::desc("Auxiliary filter for shared object symbol table"),
  llvm::cl::value_desc("name"));

llvm::cl::alias ArgAuxiliaryAlias("auxiliary",
  llvm::cl::desc("alias for -f"),
  llvm::cl::aliasopt(ArgAuxiliary));

llvm::cl::opt<std::string> ArgFilter("F",
  llvm::cl::desc("Filter for shared object symbol table"),
  llvm::cl::value_desc("name"));

llvm::cl::alias ArgFilterAlias("filter",
  llvm::cl::desc("alias for -F"),
  llvm::cl::aliasopt(ArgFilter));

// } Not supported yet

} // anonymous namespace

using namespace mcld;

//===----------------------------------------------------------------------===//
// DynamicSectionOptions
//===----------------------------------------------------------------------===//
DynamicSectionOptions::DynamicSectionOptions()
  : m_Entry(ArgEntry),
    m_Bsymbolic(ArgBsymbolic),
    m_Bgroup(ArgBgroup),
    m_SOName(ArgSOName),
    m_NoUndefined(ArgNoUndefined),
    m_AllowMulDefs(ArgAllowMulDefs),
    m_ZOptionList(ArgZOptionList),
    m_Dyld(ArgDyld),
    m_EnableNewDTags(ArgEnableNewDTags),
    m_Auxiliary(ArgAuxiliary),
    m_Filter(ArgFilter) {
}

bool DynamicSectionOptions::parse(LinkerConfig& pConfig, LinkerScript& pScript)
{
  // set up entry point from -e
  pScript.setEntry(m_Entry);

  // --Bsymbolic
  pConfig.options().setBsymbolic(m_Bsymbolic);

  // --Bgroup
  pConfig.options().setBgroup(m_Bgroup);

  // set --soname [soname]
  pConfig.options().setSOName(m_SOName);

  // set -z options
  llvm::cl::list<ZOption>::iterator zOpt;
  llvm::cl::list<ZOption>::iterator zOptEnd = m_ZOptionList.end();
  for (zOpt = m_ZOptionList.begin(); zOpt != zOptEnd; ++zOpt) {
    pConfig.options().addZOption(*zOpt);
  }

  // set --no-undefined
  if (llvm::cl::BOU_UNSET != m_NoUndefined)
    pConfig.options().setNoUndefined(llvm::cl::BOU_TRUE == m_NoUndefined);

  // set --allow-multiple-definition
  if (llvm::cl::BOU_UNSET != m_AllowMulDefs)
    pConfig.options().setMulDefs(llvm::cl::BOU_TRUE == m_AllowMulDefs);

  // set --dynamic-linker [dyld]
  pConfig.options().setDyld(m_Dyld);

  // set --enable-new-dtags
  pConfig.options().setNewDTags(m_EnableNewDTags);

  // set --auxiliary, -f
  llvm::cl::list<std::string>::iterator aux;
  llvm::cl::list<std::string>::iterator auxEnd = m_Auxiliary.end();
  for (aux = m_Auxiliary.begin(); aux != auxEnd; ++aux)
    pConfig.options().getAuxiliaryList().push_back(*aux);

  // set --filter, -F
  pConfig.options().setFilter(m_Filter);

  return true;
}

