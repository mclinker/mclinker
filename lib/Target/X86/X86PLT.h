//===- X86PLT.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_X86_PLT_H
#define MCLD_TARGET_X86_PLT_H

#include <mcld/Target/PLT.h>

namespace mcld {

class X86GOTPLT;
class GOTEntry;
class LinkerConfig;

class X86PLT0 : public PLTEntry
{
public:
  X86PLT0(SectionData* pParent, unsigned int pSize);
};

class X86PLT1 : public PLTEntry
{
public:
  X86PLT1(SectionData* pParent, unsigned int pSize);
};

/** \class X86PLT
 *  \brief X86 Procedure Linkage Table
 */
class X86PLT : public PLT
{
public:
  X86PLT(LDSection& pSection,
         SectionData& pSectionData,
         X86GOTPLT& pGOTPLT,
         const LinkerConfig& pConfig);
  ~X86PLT();

  // finalizeSectionSize - set LDSection size
  void finalizeSectionSize();

  // hasPLT1 - return if this PLT has any PLT1 entry
  bool hasPLT1() const;

  void reserveEntry(size_t pNum = 1) ;

  PLTEntry* getOrConsumeEntry(const ResolveInfo& pSymbol, bool& pExist) ;

  GOTEntry* getOrConsumeGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist);

  X86PLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

private:
  typedef llvm::DenseMap<const ResolveInfo*, X86PLT1*> SymbolIndexType;

private:
  X86GOTPLT& m_GOTPLT;

  // Used by getEntry() for mapping a ResolveInfo
  // instance to a PLT1 Entry.
  iterator m_PLTEntryIterator;

  SymbolIndexType m_PLTEntryMap;

  const uint8_t *m_PLT0;
  const uint8_t *m_PLT1;
  unsigned int m_PLT0Size;
  unsigned int m_PLT1Size;

  const LinkerConfig& m_Config;
};

} // namespace of mcld

#endif
