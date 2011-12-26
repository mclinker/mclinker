//===- ARMPLT.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_PLT_H
#define ARM_PLT_H

#include "mcld/Target/PLT.h"

namespace mcld {

class ARMGOT;

class ARMPLT0 : public PLTEntry {
public:
  ARMPLT0();
};

class ARMPLT1 : public PLTEntry {
public:
  ARMPLT1();
};

/** \class ARMPLT
 *  \brief ARM Procedure Linkage Table
 */
class ARMPLT : public PLT
{
  typedef llvm::DenseMap<const ResolveInfo*, ARMPLT1*> SymbolIndexType;
  typedef llvm::MCSectionData::iterator MCFragmentIteratorType;

public:
  ARMPLT(llvm::MCSectionData& pSectionData, ARMGOT& pGOT);
  ~ARMPLT();

public:

  // Override virtual function.
  // Additionally, reserveEntry is ARMGOT friend function.
  void reserveEntry(int pNum = 1) ;

  // Override virtual function.
  PLTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist) ;

public:
  ARMPLT0* getPLT0() const;

  void applyPLT0(const uint32_t pOffset);

  void applyPLT1(const uint32_t pOffset, const ResolveInfo& pInfo);

private:
  ARMGOT& m_GOT;

  // Used by getEntry() for mapping a ResolveInfo
  // instance to a PLT1 Entry.
  MCFragmentIteratorType m_MCFragmentIterator;

  SymbolIndexType m_SymbolIndexMap;
};

} // namespace of mcld

#endif

