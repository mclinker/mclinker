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
}

Module::Module(const std::string& pName)
  : m_Name(pName), m_NamePool(1024) {
}

Module::~Module()
{
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

