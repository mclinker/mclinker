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

#include <mcld/Target/GOT.h>
#include <mcld/Target/PLT.h>
#include <mcld/Target/SymbolEntryMap.h>

namespace mcld {

class ARMGOT;
class MemoryRegion;

class ARMPLT0 : public PLT::Entry {
public:
  ARMPLT0(SectionData& pParent);
};

class ARMPLT1 : public PLT::Entry {
public:
  ARMPLT1(SectionData& pParent);
};

/** \class ARMPLT
 *  \brief ARM Procedure Linkage Table
 */
class ARMPLT : public PLT
{
public:
  typedef SymbolEntryMap<GOT::Entry> SymGOTMap;
public:
  ARMPLT(LDSection& pSection, SectionData& pSectionData, ARMGOT& pGOTPLT);
  ~ARMPLT();

  // finalizeSectionSize - set LDSection size
  void finalizeSectionSize();

  // hasPLT1 - return if this plt section has any plt1 entry
  bool hasPLT1() const;

  void reserveEntry(size_t pNum = 1) ;

  PLT::Entry* getOrConsumeEntry(const ResolveInfo& pSymbol, bool& pExist) ;

  GOT::Entry* getOrConsumeGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist);

  ARMPLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

  uint64_t emit(MemoryRegion& pRegion);

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

private:
  typedef llvm::DenseMap<const ResolveInfo*, ARMPLT1*> SymbolIndexType;

private:
  ARMGOT& m_GOT;

  // Used by getEntry() for mapping a ResolveInfo instance to a PLT1 Entry.
  iterator m_PLTEntryIterator;

  SymbolIndexType m_PLTEntryMap;
  SymGOTMap m_SymGOTMap;
};

} // namespace of mcld

#endif

