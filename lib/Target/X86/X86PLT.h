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

class X86PLT0 : public PLT::Entry
{
public:
  X86PLT0(SectionData& pParent, unsigned int pSize);
};

class X86PLT1 : public PLT::Entry
{
public:
  X86PLT1(SectionData& pParent, unsigned int pSize);
};

/** \class X86PLT
 *  \brief X86 Procedure Linkage Table
 */
class X86PLT : public PLT
{
public:
  X86PLT(LDSection& pSection,
         X86GOTPLT& pGOTPLT,
         const LinkerConfig& pConfig);
  ~X86PLT();

  // finalizeSectionSize - set LDSection size
  void finalizeSectionSize();

  // hasPLT1 - return if this PLT has any PLT1 entry
  bool hasPLT1() const;

  void reserveEntry(size_t pNum = 1) ;

  PLT::Entry* consume();

  void applyPLT0();

  void applyPLT1();

  unsigned int getPLT0Size() const { return m_PLT0Size; }
  unsigned int getPLT1Size() const { return m_PLT1Size; }

private:
  X86PLT0* getPLT0() const;

private:
  X86GOTPLT& m_GOTPLT;

  // the last consumed entry.
  SectionData::iterator m_Last;

  const uint8_t *m_PLT0;
  const uint8_t *m_PLT1;
  unsigned int m_PLT0Size;
  unsigned int m_PLT1Size;

  const LinkerConfig& m_Config;
};

} // namespace of mcld

#endif
