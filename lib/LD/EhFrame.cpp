//===- EhFrame.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/SectionData.h>
#include <mcld/LD/Layout.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/Support/MemoryRegion.h>
#include <llvm/Support/Dwarf.h>

using namespace mcld;
using namespace llvm::dwarf;

//===----------------------------------------------------------------------===//
// EhFrame::CIE
//===----------------------------------------------------------------------===//
EhFrame::CIE::CIE(MemoryRegion& pRegion)
  : RegionFragment(pRegion) {
}

//===----------------------------------------------------------------------===//
// EhFrame::FDE
//===----------------------------------------------------------------------===//
EhFrame::FDE::FDE(MemoryRegion& pRegion,
                  const EhFrame::CIE& pCIE,
                  uint32_t pFileOffset,
                  uint32_t pDataStart)
  : RegionFragment(pRegion),
    m_CIE(pCIE),
    m_FileOffset(pFileOffset),
    m_DataStart(pDataStart) {
}

/// getPCBegin - return the address of FDE's pc
/// @ref binutils gold: ehframe.cc:222
uint32_t EhFrame::FDE::getPCBegin(const EhFrame& pOutput)
{
  uint8_t fde_encoding = m_CIE.getFDEEncode();
  unsigned int eh_value = fde_encoding & 0x7;

  // check the size to read in
  if (eh_value == llvm::dwarf::DW_EH_PE_absptr) {
    eh_value = DW_EH_PE_udata4;
  }

  size_t pc_size = 0x0;
  switch (eh_value) {
    case DW_EH_PE_udata2:
      pc_size = 2;
      break;
    case DW_EH_PE_udata4:
      pc_size = 4;
      break;
    case DW_EH_PE_udata8:
      pc_size = 8;
      break;
    default:
      // TODO
      break;
  }

  SizeTraits<32>::Address pc = 0x0;
  const uint8_t* offset = (const uint8_t*) getRegion().start() + m_DataStart;
  std::memcpy(&pc, offset, pc_size);

  // adjust the signed value
  bool is_signed = (fde_encoding & llvm::dwarf::DW_EH_PE_signed) != 0x0;
  if (DW_EH_PE_udata2 == eh_value && is_signed)
    pc = (pc ^ 0x8000) - 0x8000;

  // handle eh application
  switch (fde_encoding & 0x70)
  {
    case DW_EH_PE_absptr:
      break;
    case DW_EH_PE_pcrel:
      pc += pOutput.getSection().addr() + m_FileOffset + m_DataStart;
      break;
    case DW_EH_PE_datarel:
      // TODO
      break;
    default:
      // TODO
      break;
  }

  return pc;
}

//===----------------------------------------------------------------------===//
// EhFrame
//===----------------------------------------------------------------------===//
EhFrame::EhFrame(LDSection& pSection, Layout& pLayout)
  : m_Section(pSection), m_Layout(pLayout) {
}

EhFrame::~EhFrame()
{
  // Do nothing. Since all CIEs, FDEs and regular fragments are stored in
  // iplist, iplist will delete the fragments and we do not need to handle with
  // it.
}

void EhFrame::addFragment(RegionFragment& pFrag, LDSection& pSection)
{
  size_t size =  m_Layout.appendFragment(pFrag,
                                         *m_Section.getSectionData(),
                                         pSection.align());

  m_Section.setSize(m_Section.size() + size);
}

void EhFrame::addCIE(EhFrame::CIE& pCIE)
{
  m_CIEs.push_back(&pCIE);
}

void EhFrame::addFDE(EhFrame::FDE& pFDE)
{
  m_FDEs.push_back(&pFDE);
}

