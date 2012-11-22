//===- X86PLT.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "X86GOTPLT.h"
#include "X86PLT.h"

#include <llvm/Support/ELF.h>
#include <llvm/Support/Casting.h>

#include <mcld/LD/LDSection.h>
#include <mcld/LinkerConfig.h>
#include <mcld/Support/MsgHandling.h>

//===----------------------------------------------------------------------===//
// PLT entry data
//===----------------------------------------------------------------------===//
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

}

namespace mcld {

X86PLT0::X86PLT0(SectionData& pParent, unsigned int pSize)
  : PLT::Entry(pSize, pParent) { }

X86PLT1::X86PLT1(SectionData& pParent, unsigned int pSize)
  : PLT::Entry(pSize, pParent) { }

//===----------------------------------------------------------------------===//
// X86PLT
//===----------------------------------------------------------------------===//
X86PLT::X86PLT(LDSection& pSection,
               X86GOTPLT &pGOTPLT,
               const LinkerConfig& pConfig)
  : PLT(pSection),
    m_GOTPLT(pGOTPLT),
    m_Config(pConfig)
{
  assert(LinkerConfig::DynObj == m_Config.codeGenType() ||
         LinkerConfig::Exec == m_Config.codeGenType());

  if (LinkerConfig::DynObj == m_Config.codeGenType()) {
      m_PLT0 = x86_dyn_plt0;
      m_PLT1 = x86_dyn_plt1;
      m_PLT0Size = sizeof (x86_dyn_plt0);
      m_PLT1Size = sizeof (x86_dyn_plt1);
  }
  else {
      m_PLT0 = x86_exec_plt0;
      m_PLT1 = x86_exec_plt1;
      m_PLT0Size = sizeof (x86_exec_plt0);
      m_PLT1Size = sizeof (x86_exec_plt1);
  }
  new X86PLT0(*m_SectionData, m_PLT0Size);

  m_Last = m_SectionData->begin();
}

X86PLT::~X86PLT()
{
}

void X86PLT::finalizeSectionSize()
{
  uint64_t size = 0;
  // plt0 size
  size = getPLT0()->getEntrySize();

  // get first plt1 entry
  X86PLT::iterator it = begin();
  ++it;
  if (end() != it) {
    // plt1 size
    X86PLT1* plt1 = &(llvm::cast<X86PLT1>(*it));
    size += (m_SectionData->size() - 1) * plt1->getEntrySize();
  }
  m_Section.setSize(size);

  uint32_t offset = 0;
  SectionData::iterator frag, fragEnd = m_SectionData->end();
  for (frag = m_SectionData->begin(); frag != fragEnd; ++frag) {
    frag->setOffset(offset);
    offset += frag->size();
  }
}

bool X86PLT::hasPLT1() const
{
  return (m_SectionData->size() > 1);
}

void X86PLT::reserveEntry(size_t pNum)
{
  X86PLT1* plt1_entry = 0;

  for (size_t i = 0; i < pNum; ++i) {
    plt1_entry = new (std::nothrow) X86PLT1(*m_SectionData, m_PLT1Size);

    if (!plt1_entry)
      fatal(diag::fail_allocate_memory_plt);
  }
}

PLT::Entry* X86PLT::consume()
{
  // This will skip PLT0.
  ++m_Last;
  assert(m_Last != m_SectionData->end() &&
         "The number of PLT Entries and ResolveInfo doesn't match");
  return llvm::cast<X86PLT1>(&(*m_Last));
}

X86PLT0* X86PLT::getPLT0() const
{

  iterator first = m_SectionData->getFragmentList().begin();

  assert(first != m_SectionData->getFragmentList().end() &&
         "FragmentList is empty, getPLT0 failed!");

  X86PLT0* plt0 = &(llvm::cast<X86PLT0>(*first));

  return plt0;
}

// FIXME: It only works on little endian machine.
void X86PLT::applyPLT0()
{
  X86PLT0* plt0 = getPLT0();

  unsigned char* data = 0;
  data = static_cast<unsigned char*>(malloc(plt0->getEntrySize()));

  if (!data)
    fatal(diag::fail_allocate_memory_plt);

  memcpy(data, m_PLT0, plt0->getEntrySize());

  if (m_PLT0 == x86_exec_plt0) {
    uint32_t *offset = reinterpret_cast<uint32_t*>(data + 2);
    *offset = m_GOTPLT.addr() + 4;
    offset = reinterpret_cast<uint32_t*>(data + 8);
    *offset = m_GOTPLT.addr() + 8;
  }

  plt0->setContent(data);
}

// FIXME: It only works on little endian machine.
void X86PLT::applyPLT1()
{
  assert(m_Section.addr() && ".plt base address is NULL!");

  X86PLT::iterator it = m_SectionData->begin();
  X86PLT::iterator ie = m_SectionData->end();
  assert(it != ie && "FragmentList is empty, applyPLT1 failed!");

  uint64_t GOTEntrySize = m_GOTPLT.getEntrySize();

  // Skip GOT0
  uint64_t GOTEntryOffset = GOTEntrySize * X86GOTPLT0Num;
  if (LinkerConfig::Exec == m_Config.codeGenType())
    GOTEntryOffset += m_GOTPLT.addr();

  //skip PLT0
  uint64_t PLTEntryOffset = m_PLT0Size;
  ++it;

  X86PLT1* plt1 = 0;

  uint64_t PLTRelOffset = 0;

  while (it != ie) {
    plt1 = &(llvm::cast<X86PLT1>(*it));
    unsigned char *data;
    data = static_cast<unsigned char*>(malloc(plt1->getEntrySize()));

    if (!data)
      fatal(diag::fail_allocate_memory_plt);

    memcpy(data, m_PLT1, plt1->getEntrySize());

    uint32_t* offset;

    offset = reinterpret_cast<uint32_t*>(data + 2);
    *offset = GOTEntryOffset;
    GOTEntryOffset += GOTEntrySize;

    offset = reinterpret_cast<uint32_t*>(data + 7);
    *offset = PLTRelOffset;
    PLTRelOffset += sizeof (llvm::ELF::Elf32_Rel);

    offset = reinterpret_cast<uint32_t*>(data + 12);
    *offset = -(PLTEntryOffset + 12 + 4);
    PLTEntryOffset += m_PLT1Size;

    plt1->setContent(data);
    ++it;
  }
}

} // end namespace mcld

