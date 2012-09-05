//===- OutputRelocSection.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OUTPUT_RELOCATION_SECTION_H
#define MCLD_OUTPUT_RELOCATION_SECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/SectionData.h>

namespace mcld
{

class Relocation;
class RelocationFactory;

/** \class OutputRelocSection
 *  \brief Dynamic relocation section for ARM .rel.dyn and .rel.plt
 */
class OutputRelocSection
{
public:
  OutputRelocSection(LDSection& pSection,
                     SectionData& pSectionData,
                     unsigned int pEntrySize);

  ~OutputRelocSection();

  void reserveEntry(RelocationFactory& pRelFactory, size_t pNum=1);

  Relocation* consumeEntry();

private:
  typedef SectionData::iterator FragmentIterator;

private:
  /// m_pSection - LDSection of this Section
  LDSection* m_pSection;

  /// m_SectionData - SectionData which contains the dynamic relocations
  SectionData* m_pSectionData;

  /// m_EntryBytes - size of a relocation entry
  unsigned int m_EntryBytes;

  /// m_isVisit - First time visit the function getEntry() or not
  bool m_isVisit;

  /// m_ValidEntryIterator - point to the first valid entry
  FragmentIterator m_ValidEntryIterator;
};

} // namespace of mcld

#endif

