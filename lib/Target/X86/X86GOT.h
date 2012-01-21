//===- X86GOT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_X86_GOT_H
#define MCLD_X86_GOT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "X86PLT.h"
#include <mcld/Target/GOT.h>

namespace mcld
{
class LDSection;

/** \class X86GOT
 *  \brief X86 Global Offset Table.
 */

const unsigned int X86GOT0Num = 3;

class X86GOT : public GOT
{
  friend void mcld::X86PLT::reserveEntry(int pNum);

  friend mcld::PLTEntry* mcld::X86PLT::getPLTEntry(
         const mcld::ResolveInfo& pSymbol,bool& pExist);

  friend mcld::GOTEntry* mcld::X86PLT::getGOTPLTEntry(
         const mcld::ResolveInfo& pSymbol,bool& pExist);

  typedef llvm::DenseMap<const ResolveInfo*, GOTEntry*> SymbolIndexMapType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  X86GOT(LDSection& pSection, llvm::MCSectionData& pSectionData);

  ~X86GOT();

  //Reserve general GOT entries.
  void reserveEntry(size_t pNum = 1);

  GOTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist);

  void applyGOT0(uint64_t pAddress);

  iterator begin();

  const_iterator begin() const;

  iterator end();

  const_iterator end() const;

  unsigned int getGOTPLTNum() const;

  iterator getLastGOT0();

  const iterator getLastGOT0() const;

private:

  unsigned int m_GeneralGOTNum;
  unsigned int m_GOTPLTNum;

  // Used by getGeneralGOTEntry()
  iterator m_GeneralGOTIterator;

  // Used by getGOTPLTEntry()
  iterator m_GOTPLTIterator;

  // The last GOT0 entry
  iterator m_LastGOT0;

  SymbolIndexMapType m_GOTPLTMap;
  SymbolIndexMapType m_GeneralGOTMap;
};

} // namespace of mcld

#endif

