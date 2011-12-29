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
class GOTEntry;

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

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  ARMPLT(LDSection& pSection,
         llvm::MCSectionData& pSectionData,
         ARMGOT& pGOTPLT);
  ~ARMPLT();

// Override virtual function.
public:

  // reserveEntry is ARMGOT friend function.
  void reserveEntry(int pNum = 1) ;

  PLTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist) ;

  GOTEntry* getGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist);

public:

  iterator begin() { return m_pSectionData->begin(); }

  const_iterator begin() const { return m_pSectionData->begin(); }

  iterator end() { return m_pSectionData->end(); }

  const_iterator end() const { return m_pSectionData->end(); }

  ARMPLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

private:
  ARMGOT& m_GOTPLT;

  // Used by getEntry() for mapping a ResolveInfo
  // instance to a PLT1 Entry.
  iterator m_MCFragmentIterator;

  SymbolIndexType m_SymbolIndexMap;
};

} // namespace of mcld

#endif

