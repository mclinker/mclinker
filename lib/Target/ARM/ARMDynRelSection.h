//===- ARMDynRelSection.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARMDYNRELSECTION_H
#define MCLD_ARMDYNRELSECTION_H
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

/** \class ARMDynRelSection
 *  \brief Dynamic relocation section for ARM .rel.dyn and .rel.plt
 */
class ARMDynRelSection
{
public:
  typedef llvm::DenseMap<const ResolveInfo*, Relocation*> SymRelMapType;
  typedef SymRelMapType::iterator SymRelMapIterator;

  typedef llvm::MCSectionData::iterator MCFragmentIterator;

public:
  ARMDynRelSection(llvm::MCSectionData& pSectionData);
  ~ARMDynRelSection();

  void reserveEntry(RelocationFactory& pRelFactory, int pNum=1);
  Relocation* getEntry(const ResolveInfo& pSymbol, bool& pExist);

private:
  /// m_pEmpty - point to the top empty entry
  MCFragmentIterator m_pEmpty;

  /// m_SymRelMap - map the resolved symbol to the Relocation entry
  SymRelMapType m_SymRelMap;

  /// m_SectionData - MCSectionData which contains the dynamic relocations
  llvm::MCSectionData* m_pSectionData;
};

} // namespace of mcld

#endif

