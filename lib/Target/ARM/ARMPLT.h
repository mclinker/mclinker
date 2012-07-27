//===- ARMPLT.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_PLT_H
#define MCLD_ARM_PLT_H

#include <mcld/LD/SectionData.h>
#include <mcld/Target/PLT.h>

namespace mcld {

class ARMGOT;
class GOTEntry;
class MemoryRegion;

class ARMPLT0 : public PLTEntry {
public:
  ARMPLT0(SectionData* pParent);
};

class ARMPLT1 : public PLTEntry {
public:
  ARMPLT1(SectionData* pParent);
};

/** \class ARMPLT
 *  \brief ARM Procedure Linkage Table
 */
class ARMPLT : public PLT
{
  typedef llvm::DenseMap<const ResolveInfo*, ARMPLT1*> SymbolIndexType;

public:
  typedef SectionData::iterator iterator;
  typedef SectionData::const_iterator const_iterator;

public:
  ARMPLT(LDSection& pSection, SectionData& pSectionData, ARMGOT& pGOTPLT);
  ~ARMPLT();

// Override virtual function.
public:

  // reserveEntry is ARMGOT friend function.
  void reserveEntry(size_t pNum = 1) ;

  PLTEntry* getPLTEntry(const ResolveInfo& pSymbol, bool& pExist) ;

  GOTEntry* getGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist);

public:
  iterator begin() { return m_SectionData.begin(); }

  const_iterator begin() const { return m_SectionData.begin(); }

  iterator end() { return m_SectionData.end(); }

  const_iterator end() const { return m_SectionData.end(); }

  ARMPLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

  uint64_t emit(MemoryRegion& pRegion);

private:
  ARMGOT& m_GOT;

  // Used by getEntry() for mapping a ResolveInfo
  // instance to a PLT1 Entry.
  iterator m_PLTEntryIterator;

  SymbolIndexType m_PLTEntryMap;
};

} // namespace of mcld

#endif

