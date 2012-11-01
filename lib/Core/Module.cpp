//===- Module.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Module.h>
#include <mcld/Fragment/FragmentRef.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/NamePool.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/StaticResolver.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Module
//===----------------------------------------------------------------------===//
Module::Module()
  : m_NamePool(1024) {
  m_pSectionSymbolMap = new SectHashTableType(16);
}

Module::Module(const std::string& pName)
  : m_Name(pName), m_NamePool(1024) {
  m_pSectionSymbolMap = new SectHashTableType(16);
}

Module::~Module()
{
  delete m_pSectionSymbolMap;
}

// Following two functions will be obsolette when we have new section merger.
LDSection* Module::getSection(const std::string& pName)
{
  iterator sect, sectEnd = end();
  for (sect = begin(); sect != sectEnd; ++sect) {
    if ((*sect)->name() == pName)
      return *sect;
  }
  return NULL;
}

const LDSection* Module::getSection(const std::string& pName) const
{
  const_iterator sect, sectEnd = end();
  for (sect = begin(); sect != sectEnd; ++sect) {
    if ((*sect)->name() == pName)
      return *sect;
  }
  return NULL;
}

LDSymbol* Module::getSectionSymbol(const LDSection* pSection)
{
  SectHashTableType::iterator entry = m_pSectionSymbolMap->find(pSection);
  return entry.getEntry()->value();
}

const LDSymbol* Module::getSectionSymbol(const LDSection* pSection) const
{
  SectHashTableType::iterator entry = m_pSectionSymbolMap->find(pSection);
  return entry.getEntry()->value();
}

bool Module::addSectionSymbol(LDSection& pOutputSection)
{
  // create the resolveInfo for this section symbol
  llvm::StringRef sym_name = llvm::StringRef(pOutputSection.name());
  ResolveInfo* sym_info = m_NamePool.createSymbol(sym_name,
                                                  false,
                                                  ResolveInfo::Section,
                                                  ResolveInfo::Define,
                                                  ResolveInfo::Local,
                                                  0x0, // size
                                                  ResolveInfo::Default);
  // create the output section symbol and set its fragRef to the first fragment
  // of the section
  LDSymbol* sym = LDSymbol::Create(*sym_info);
  SectionData* sd = NULL;
  switch (pOutputSection.kind()) {
    case LDFileFormat::Relocation:
      return false;
    case LDFileFormat::EhFrame:
      sd = &pOutputSection.getEhFrame()->getSectionData();
      break;
    default:
      sd = pOutputSection.getSectionData();
      break;
  }
  assert(!sd->empty());
  FragmentRef* frag_ref = FragmentRef::Create(sd->front(), 0x0);
  sym->setFragmentRef(frag_ref);
  sym_info->setSymPtr(sym);

  // push symbol into output symbol table
  m_SymbolTable.add(*sym);

  // insert the symbol to the Section to Symbol hash map
  bool exist = false;
  SectHashTableType::entry_type* entry =
                            m_pSectionSymbolMap->insert(&pOutputSection, exist);
  assert(!exist);
  entry->setValue(sym);
  return true;
}

