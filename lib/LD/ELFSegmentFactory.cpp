//===- ELFSegmentFactory.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFSegmentFactory.h>

using namespace mcld;

//==========================
// ELFSegmentFactory

ELFSegmentFactory::ELFSegmentFactory(size_t pNum)
  : GCFactory<ELFSegment, 0>(pNum)
{
}

ELFSegmentFactory::~ELFSegmentFactory()
{
}

/// produce - produce an empty ELF segment information.
/// this function will create an ELF segment
/// @param pType - p_type in ELF program header
ELFSegment* ELFSegmentFactory::produce(uint32_t pType)
{
  ELFSegment* segment = allocate();
  new (segment) ELFSegment(pType);
  return segment;
}

/// destroy - destruct the ELF segment
void ELFSegmentFactory::destroy(ELFSegment*& pSegment)
{
  deallocate(pSegment);
}

