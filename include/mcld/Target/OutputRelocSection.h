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

#include <llvm/ADT/DenseMap.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/RelocationFactory.h>

namespace mcld
{

class ResolveInfo;
class Relocation;

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

  Relocation* getEntry(const ResolveInfo& pSymbol,
                       bool isForGOT,
                       bool& pExist);

private:
  typedef llvm::DenseMap<const ResolveInfo*, Relocation*> SymRelMapType;

  typedef SymRelMapType::iterator SymRelMapIterator;

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

  /// m_SymRelMap - map the resolved symbol to the Relocation entry
  SymRelMapType m_SymRelMap;
};

} // namespace of mcld

#endif

