//===- SectionFactory.cpp -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/SectionFactory.h>

using namespace mcld;

//==========================
// SectionFactory
SectionFactory::SectionFactory(size_t pNum)
  : GCFactory<LDSection, 0>(pNum) {
}

SectionFactory::~SectionFactory()
{
}

LDSection* SectionFactory::produce(const std::string& pName,
                                   LDFileFormat::Kind pKind,
                                   uint32_t pType,
                                   uint32_t pFlag)
{
  // create a LDSection
  LDSection* result = allocate();
  new (result) LDSection(pName, pKind, pType, pFlag);
  return result;
}

void SectionFactory::destroy(LDSection*& pSection)
{
  // do not recycle LDSection. HeaderFactory will do that job.
  deallocate(pSection);
}

