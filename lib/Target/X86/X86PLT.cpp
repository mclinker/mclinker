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

#include <new>

#include <llvm/Support/ELF.h>
#include <llvm/Support/Casting.h>

#include <mcld/MC/MCLDOutput.h>
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

X86PLT0::X86PLT0(SectionData* pParent, unsigned int pSize)
  : PLTEntry(pSize, pParent) { }

X86PLT1::X86PLT1(SectionData* pParent, unsigned int pSize)
  : PLTEntry(pSize, pParent) { }

//===----------------------------------------------------------------------===//
// X86PLT
//===----------------------------------------------------------------------===//
X86PLT::X86PLT(LDSection& pSection,
               SectionData& pSectionData,
               X86GOTPLT &pGOTPLT,
               const Output& pOutput)
  : PLT(pSection, pSectionData),
    m_GOTPLT(pGOTPLT),
    m_PLTEntryIterator(),
    m_Output(pOutput)
{
  assert (Output::DynObj == pOutput.type() || Output::Exec == pOutput.type());
  if (Output::DynObj == pOutput.type()) {
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
  X86PLT0* plt0_entry = new X86PLT0(&m_SectionData, m_PLT0Size);

  m_Section.setSize(m_Section.size() + plt0_entry->getEntrySize());

  m_PLTEntryIterator = pSectionData.begin();
}

X86PLT::~X86PLT()
{
}

void X86PLT::reserveEntry(size_t pNum)
{
  X86PLT1* plt1_entry = 0;

  for (size_t i = 0; i < pNum; ++i) {
    plt1_entry = new (std::nothrow) X86PLT1(&m_SectionData, m_PLT1Size);

    if (!plt1_entry)
      fatal(diag::fail_allocate_memory_plt);

    m_Section.setSize(m_Section.size() + plt1_entry->getEntrySize());

    // reserve corresponding entry in .got.plt
    m_GOTPLT.reserveEntry(pNum);
  }
}

PLTEntry* X86PLT::getPLTEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   X86PLT1 *&PLTEntry = m_PLTEntryMap[&pSymbol];

   pExist = 1;

   if (!PLTEntry) {
     pExist = 0;

     // This will skip PLT0.
     ++m_PLTEntryIterator;
     assert(m_PLTEntryIterator != m_SectionData.end() &&
            "The number of PLT Entries and ResolveInfo doesn't match");
     PLTEntry = llvm::cast<X86PLT1>(&(*m_PLTEntryIterator));
   }
   return PLTEntry;
}

GOTEntry* X86PLT::getGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist)
{
   return m_GOTPLT.getEntry(pSymbol, pExist);
}

X86PLT0* X86PLT::getPLT0() const {

  iterator first = m_SectionData.getFragmentList().begin();

  assert(first != m_SectionData.getFragmentList().end() &&
         "FragmentList is empty, getPLT0 failed!");

  X86PLT0* plt0 = &(llvm::cast<X86PLT0>(*first));

  return plt0;
}

// FIXME: It only works on little endian machine.
void X86PLT::applyPLT0() {

  iterator first = m_SectionData.getFragmentList().begin();

  assert(first != m_SectionData.getFragmentList().end() &&
         "FragmentList is empty, applyPLT0 failed!");

  X86PLT0* plt0 = &(llvm::cast<X86PLT0>(*first));

  unsigned char* data = 0;
  data = static_cast<unsigned char*>(malloc(plt0->getEntrySize()));

  if (!data)
    fatal(diag::fail_allocate_memory_plt);

  memcpy(data, m_PLT0, plt0->getEntrySize());

  if (m_PLT0 == x86_exec_plt0) {
    uint64_t got_base = m_GOTPLT.getSection().addr();
    assert(got_base && ".got base address is NULL!");
    uint32_t *offset = reinterpret_cast<uint32_t*>(data + 2);
    *offset = got_base + 4;
    offset = reinterpret_cast<uint32_t*>(data + 8);
    *offset = got_base + 8;
  }

  plt0->setContent(data);
}

// FIXME: It only works on little endian machine.
void X86PLT::applyPLT1() {

  uint64_t plt_base = m_Section.addr();
  assert(plt_base && ".plt base address is NULL!");

  uint64_t got_base = m_GOTPLT.getSection().addr();
  assert(got_base && ".got base address is NULL!");

  X86PLT::iterator it = m_SectionData.begin();
  X86PLT::iterator ie = m_SectionData.end();
  assert(it != ie && "FragmentList is empty, applyPLT1 failed!");

  uint64_t GOTEntrySize = m_GOTPLT.getEntrySize();

  // Skip GOT0
  uint64_t GOTEntryOffset = GOTEntrySize * X86GOTPLT0Num;

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
    if (m_Output.type() == Output::DynObj) {
      *offset = GOTEntryOffset;
    } else {
      // Exec
      *offset = got_base + GOTEntryOffset;
    }
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

  // apply .got.plt
  m_GOTPLT.applyAllGOTPLT(plt_base, m_PLT0Size, m_PLT1Size);
}

} // end namespace mcld

