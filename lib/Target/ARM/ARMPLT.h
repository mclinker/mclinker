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
public:
  ARMPLT(LDSection& pSection, SectionData& pSectionData, ARMGOT& pGOTPLT);
  ~ARMPLT();

  // finalizeSectionSize - set LDSection size
  void finalizeSectionSize();

  // hasPLT1 - return if this plt section has any plt1 entry
  bool hasPLT1() const;

  void reserveEntry(size_t pNum = 1) ;

  PLTEntry* getOrConsumeEntry(const ResolveInfo& pSymbol, bool& pExist) ;

  GOTEntry* getOrConsumeGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist);

  ARMPLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

  uint64_t emit(MemoryRegion& pRegion);

private:
  typedef llvm::DenseMap<const ResolveInfo*, ARMPLT1*> SymbolIndexType;

private:
  ARMGOT& m_GOT;

  // Used by getEntry() for mapping a ResolveInfo instance to a PLT1 Entry.
  iterator m_PLTEntryIterator;

  SymbolIndexType m_PLTEntryMap;
};

} // namespace of mcld

#endif

