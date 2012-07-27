//===- X86GOTPLT.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_GOTPLT_H
#define MCLD_X86_GOTPLT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/DenseMap.h>

#include <mcld/Target/GOT.h>
#include <mcld/LD/SectionData.h>

namespace mcld
{
class LDSection;

const unsigned int X86GOTPLT0Num = 3;

/** \class X86GOTPLT
 *  \brief X86 .got.plt section.
 */
class X86GOTPLT : public GOT
{
  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef SectionData::iterator iterator;
  typedef SectionData::const_iterator const_iterator;

public:
  X86GOTPLT(LDSection &pSection, SectionData& pSectionData);

  ~X86GOTPLT();

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

// For GOT0
public:
  void applyGOT0(uint64_t pAddress);

// For GOTPLT
public:
  void reserveEntry(size_t pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

  void applyAllGOTPLT(uint64_t pPLTBase,
                      unsigned int pPLT0Size,
                      unsigned int pPLT1Size);

  GOTEntry*& lookupGOTPLTMap(const ResolveInfo& pSymbol);

private:
  iterator m_GOTPLTIterator;
  SymbolIndexMapType m_GOTPLTMap;
};

} // namespace of mcld

#endif

