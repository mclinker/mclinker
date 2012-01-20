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
  friend void mcld::ARMPLT::reserveEntry(int pNum);

  friend mcld::PLTEntry* mcld::ARMPLT::getPLTEntry(
         const mcld::ResolveInfo& pSymbol,bool& pExist);

  friend mcld::GOTEntry* mcld::ARMPLT::getGOTPLTEntry(
         const mcld::ResolveInfo& pSymbol,bool& pExist);

  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  ARMGOT(LDSection &pSection, llvm::MCSectionData& pSectionData);

  ~ARMGOT();

  //Reserve general GOT entries.
  void reserveEntry(int pNum = 1);

  void reserveGOTPLTEntry();

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

  void applyGOT0(const uint64_t pAddress);

  void applyAllGOTPLT(const uint64_t pPLTBase);

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

private:
  iterator getGOTPLTBegin();

  const iterator getGOTPLTEnd();

private:
  unsigned int m_NormalGOTNum;

  // Used by getNormalGOTEntry()
  iterator m_NormalGOTIterator;

  // Used by getGOTPLTEntry()
  iterator m_GOTPLTIterator;

  iterator m_GOTPLTBegin;
  iterator m_GOTPLTEnd;

  SymbolIndexMapType m_GOTPLTMap;
  SymbolIndexMapType m_NormalGOTMap;
};

} // namespace of mcld

#endif

