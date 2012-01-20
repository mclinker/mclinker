//===- OutputRelocSection.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef OUTPUTRELOCSECTION_H
#define OUTPUTRELOCSECTION_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/MC/MCAssembler.h>
#include <llvm/ADT/DenseMap.h>
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
  typedef llvm::DenseMap<const ResolveInfo*, Relocation*> SymRelMapType;
  typedef SymRelMapType::iterator SymRelMapIterator;

  typedef llvm::MCSectionData::iterator MCFragmentIterator;

public:
  OutputRelocSection(LDSection& pSection,
                   llvm::MCSectionData& pSectionData,
                   const unsigned int pEntrySize);
  ~OutputRelocSection();

  void reserveEntry(RelocationFactory& pRelFactory, int pNum=1);

  Relocation* getEntry(const ResolveInfo& pSymbol,
                       bool isForGOT,
                       bool& pExist);

private:
  /// m_pEmpty - point to the top empty entry
  MCFragmentIterator m_pEmpty;

  /// m_SymRelMap - map the resolved symbol to the Relocation entry
  SymRelMapType m_SymRelMap;

  /// m_pSection - LDSection of this Section
  LDSection* m_pSection;

  /// m_SectionData - MCSectionData which contains the dynamic relocations
  llvm::MCSectionData* m_pSectionData;

  /// m_EntryBytes - size of a relocation entry
  const unsigned int m_EntryBytes;
};

} // namespace of mcld

#endif

