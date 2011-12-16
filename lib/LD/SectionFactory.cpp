//===- impl.cpp -----------------------------------------------------------===//
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
  : GCFactory<llvm::MCSectionData, 0>(pNum),
    m_HeaderFactory(pNum) {
}

SectionFactory::~SectionFactory()
{
}

llvm::MCSectionData* SectionFactory::produce(LDFileFormat::Kind pKind,
                                             const std::string& pName,
                                             uint32_t pFlag,
                                             uint32_t pType)
{
  // create a LDSection
  LDSection* header = m_HeaderFactory.allocate();
  new (header) LDSection(pKind, pName, pFlag, pType);

  // create a MCSectionData
  MCSectionData* result = allocate();
  new (result) MCSectionData(*header);

  return result;
}

void SectionFactory::destroy(llvm::MCSectionData* pSectionData)
{
  // do not recycle LDSection. HeaderFactory will do that job.
  deallocate(pSectionData);
}

