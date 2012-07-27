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

#include <llvm/ADT/DenseMap.h>

#include <mcld/Target/GOT.h>
#include <mcld/LD/SectionData.h>

namespace mcld
{
class LDSection;
class MemoryRegion;

/** \class ARMGOT
 *  \brief ARM Global Offset Table.
 */
class ARMGOT : public GOT
{
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef SectionData::iterator iterator;
  typedef SectionData::const_iterator const_iterator;

  enum {
    ARMGOT0Num = 3
  };

public:
  ARMGOT(LDSection &pSection, SectionData& pSectionData);

  ~ARMGOT();

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

  uint64_t emit(MemoryRegion& pRegion);
// For GOT0
public:
  void applyGOT0(uint64_t pAddress);

// For normal GOT
public:
  // Reserve normal GOT entries.
  void reserveEntry(size_t pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

// For GOTPLT
public:
  void reserveGOTPLTEntry();

  void applyAllGOTPLT(uint64_t pPLTBase);

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

