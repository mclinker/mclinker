//===- ARMGOT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_GOT_H
#define MCLD_ARM_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "ARMPLT.h"
#include "mcld/Target/GOT.h"

namespace mcld
{
class LDSection;

/** \class ARMGOT
 *  \brief ARM Global Offset Table.
 */
class ARMGOT : public GOT
{
  friend void mcld::ARMPLT::reserveEntry(int pNum);
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  ARMGOT(LDSection &pSection, llvm::MCSectionData& pSectionData);

  ~ARMGOT();

  //Reserve general GOT entries.
  void reserveEntry(int pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

  void applyGOT0(uint64_t pAddress);

  void applyGOTPLT(uint64_t pAddress);

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

private:

  unsigned int m_GeneralGOTNum;
  unsigned int m_GOTPLTNum;

  iterator m_GeneralGOTIterator;

  SymbolIndexMapType m_SymbolIndexMap;
};

} // namespace of mcld

#endif

