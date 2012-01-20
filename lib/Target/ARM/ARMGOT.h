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

const unsigned int ARMGOT0Num = 3;

class ARMGOT : public GOT
{
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  ARMGOT(LDSection &pSection, llvm::MCSectionData& pSectionData);

  ~ARMGOT();

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

// For GOT0
public:
  void applyGOT0(const uint64_t pAddress);

// For normal GOT
public:
  // Reserve normal GOT entries.
  void reserveEntry(int pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

// For GOTPLT
public:
  void reserveGOTPLTEntry();

  void applyAllGOTPLT(const uint64_t pPLTBase);

  GOTEntry*& lookupGOTPLTMap(const ResolveInfo& pSymbol);

  iterator getNextGOTPLTEntry();

  iterator getGOTPLTBegin();

  const iterator getGOTPLTEnd();

private:
  // For normal GOT entries
  iterator m_NormalGOTIterator;
  SymbolIndexMapType m_NormalGOTMap;

  // For GOTPLT entries
  iterator m_GOTPLTIterator;
  SymbolIndexMapType m_GOTPLTMap;

  iterator m_GOTPLTBegin;
  iterator m_GOTPLTEnd;
};

} // namespace of mcld

#endif

