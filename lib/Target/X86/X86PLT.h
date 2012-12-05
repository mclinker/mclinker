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

namespace {

const uint8_t x86_dyn_plt0[] = {
  0xff, 0xb3, 0x04, 0, 0, 0, // pushl  0x4(%ebx)
  0xff, 0xa3, 0x08, 0, 0, 0, // jmp    *0x8(%ebx)
  0x0f, 0x1f, 0x4,  0        // nopl   0(%eax)
};

const uint8_t x86_dyn_plt1[] = {
  0xff, 0xa3, 0, 0, 0, 0,    // jmp    *sym@GOT(%ebx)
  0x68, 0, 0, 0, 0,          // pushl  $offset
  0xe9, 0, 0, 0, 0           // jmp    plt0
};

const uint8_t x86_exec_plt0[] = {
  0xff, 0x35, 0, 0, 0, 0,    // pushl  .got + 4
  0xff, 0x25, 0, 0, 0, 0,    // jmp    *(.got + 8)
  0x0f, 0x1f, 0x4, 0         // nopl   0(%eax)
};

const uint8_t x86_exec_plt1[] = {
  0xff, 0x25, 0, 0, 0, 0,    // jmp    *(sym in .got)
  0x68, 0, 0, 0, 0,          // pushl  $offset
  0xe9, 0, 0, 0, 0           // jmp    plt0
};

} // anonymous namespace

namespace mcld {

class X86GOTPLT;
class GOTEntry;
class LinkerConfig;

//===----------------------------------------------------------------------===//
// X86PLT Entry
//===----------------------------------------------------------------------===//
class X86DynPLT0 : public PLT::Entry<sizeof(x86_dyn_plt0)>
{
public:
  X86DynPLT0(SectionData& pParent);
};

class X86DynPLT1 : public PLT::Entry<sizeof(x86_dyn_plt1)>
{
public:
  X86DynPLT1(SectionData& pParent);
};

class X86ExecPLT0 : public PLT::Entry<sizeof(x86_exec_plt0)>
{
public:
  X86ExecPLT0(SectionData& pParent);
};

class X86ExecPLT1 : public PLT::Entry<sizeof(x86_exec_plt1)>
{
public:
  X86ExecPLT1(SectionData& pParent);
};

//===----------------------------------------------------------------------===//
// X86PLT
//===----------------------------------------------------------------------===//
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

  PLTEntryBase* consume();

  void applyPLT0();

  void applyPLT1();

  unsigned int getPLT0Size() const { return m_PLT0Size; }
  unsigned int getPLT1Size() const { return m_PLT1Size; }

private:
  PLTEntryBase* getPLT0() const;

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

