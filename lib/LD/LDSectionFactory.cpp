//===- LDSectionFactory.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDSectionFactory.h>

using namespace mcld;

//==========================
// LDSectionFactory
LDSectionFactory::LDSectionFactory(size_t pNum)
  : GCFactory<LDSection, 0>(pNum) {
}

LDSectionFactory::~LDSectionFactory()
{
}

LDSection* LDSectionFactory::produce(const std::string& pName,
                                   LDFileFormat::Kind pKind,
                                   uint32_t pType,
                                   uint32_t pFlag)
{
  // create a LDSection
  LDSection* result = allocate();
  new (result) LDSection(pName, pKind, pType, pFlag);
  return result;
}

void LDSectionFactory::destroy(LDSection*& pSection)
{
  // do not recycle LDSection. HeaderFactory will do that job.
  deallocate(pSection);
}

LDSection* LDSectionFactory::find(const std::string& pName)
{
  iterator sect_iter, sect_end = end();
  for (sect_iter = begin(); sect_iter != sect_end; ++sect_iter)
    if ((*sect_iter).name() == pName)
      break;
  if (sect_iter == sect_end)
    return NULL;
  return &(*sect_iter);
}
