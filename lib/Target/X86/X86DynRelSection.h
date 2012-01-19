//===- X86DynRelSection.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86DYNRELSECTION_H
#define MCLD_X86DYNRELSECTION_H
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

/** \class X86DynRelSection
 *  \brief Dynamic relocation section for X86 .rel.dyn and .rel.plt
 */
class X86DynRelSection
{
public:
  typedef llvm::DenseMap<const ResolveInfo*, Relocation*> SymRelMapType;
  typedef SymRelMapType::iterator SymRelMapIterator;

  typedef llvm::MCSectionData::iterator MCFragmentIterator;

public:
  X86DynRelSection(LDSection& pSection,
                   llvm::MCSectionData& pSectionData,
                   const unsigned int pEntrySize);
  ~X86DynRelSection();

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

