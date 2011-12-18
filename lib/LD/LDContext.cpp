//===- LDContext.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDContext.h>
#include <mcld/LD/SectionFactory.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDSymbol.h>

using namespace mcld;

//==========================
// LDReader
LDContext::LDContext(SectionFactory& pSecFactory)
  : m_SectionFactory(pSecFactory) { // the average number of sections.
}

LDContext::~LDContext()
{
}

LDSection* LDContext::getSection(unsigned int pIdx)
{
  return m_SectionTable.at(pIdx);
}

const LDSection* LDContext::getSection(unsigned int pIdx) const
{
  return m_SectionTable.at(pIdx);
}

LDSection* LDContext::getSection(const std::string& pName)
{
  return NULL;
}

const LDSection* LDContext::getSection(const std::string& pName) const
{
  return NULL;
}

const LDSection& LDContext::getOrCreateSection(const std::string& pName,
                                               LDFileFormat::Kind pKind,
                                               uint32_t pType,
                                               uint32_t pFlag)
{
  // sequential search. (the scale of SectionTable is small.)
  sect_iterator sect, sect_end = sectEnd();
  for (sect = sectBegin(); sect != sect_end; ++sect) {
    if ((*sect)->name() == pName)
      return static_cast<const LDSection&>(**sect);
  }

  LDSection* result = getSectFactory().produce(pName, pKind, pType, pFlag);
  m_SectionTable.push_back(result);
  return static_cast<const LDSection&>(*result);
}

