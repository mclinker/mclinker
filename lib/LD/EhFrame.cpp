//===- EhFrame.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Target/TargetLDBackend.h>
#include <llvm/Support/Dwarf.h>
#include <llvm/Support/Host.h>

using namespace mcld;

//==========================
// EhFrame
EhFrame::EhFrame()
 : m_fCanRecognizeAll(true) {
}

EhFrame::~EhFrame()
{
}

uint64_t EhFrame::readEhFrame(Layout& pLayout,
                              const TargetLDBackend& pBackend,
                              llvm::MCSectionData& pSD,
                              LDSection& pSection,
                              MemoryArea& pArea)
{
  MemoryRegion* region = pArea.request(pSection.offset(),
                                       pSection.size());
  // an empty .eh_frame
  if (NULL == region) {
    note(diag::note_ehframe) << "an empty eh_frame";
    return 0;
  }

  ConstAddress eh_start = region->start();
  ConstAddress eh_end = region->end();
  ConstAddress p = eh_start;

  // read the Length filed
  uint32_t len = readVal(p, pBackend.isLittleEndian());

  // This CIE is a terminator if the Length field is 0, return 0 to handled it
  // as an ordinary input.
  if (0 == len) {
    note(diag::note_ehframe) << "a terminator";
    pArea.release(region);
    return 0;
  }

  if (0xffffffff == len) {
    debug(diag::debug_eh_unsupport) << "64-bit eh_frame";
    pArea.release(region);
    m_fCanRecognizeAll = false;
    return 0;
  }

  // record the order of the CIE and FDE fragments
  FragListType frag_list;

  while (p < eh_end) {

    if (eh_end - p < 4) {
      debug(diag::debug_eh_unsupport) << "CIE or FDE size smaller than 4";
      m_fCanRecognizeAll = false;
      break;
    }
    // read the Length field
    len = readVal(p, pBackend.isLittleEndian());
    p += 4;

    // the zero length entry should be the end of the section
    if (0 == len) {
      if (p < eh_end) {
        debug(diag::debug_eh_unsupport) << "Non-end entry with zero length";
        m_fCanRecognizeAll = false;
      }
      break;
    }
    if (0xffffffff == len) {
      debug(diag::debug_eh_unsupport) << "64-bit eh_frame";
      m_fCanRecognizeAll = false;
      break;
    }

    if (eh_end - p < 4) {
      debug(diag::debug_eh_unsupport) <<
        "CIE:ID field / FDE: CIE Pointer field";
      m_fCanRecognizeAll = false;
      break;
    }

    // compute the section offset of this entry
    uint32_t ent_offset = static_cast<uint32_t>(p - eh_start - 4);

    // get the MemoryRegion for this entry
    MemoryRegion* ent_region = pArea.request(pSection.offset() + ent_offset,
                                             len + 4);

    // create and add a CIE or FDE entry
    uint32_t id = readVal(p, pBackend.isLittleEndian());
    // CIE
    if (0 == id) {
      if (!addCIE(*ent_region, pBackend, frag_list)) {
        m_fCanRecognizeAll = false;
        pArea.release(ent_region);
        break;
      }
    }

    // FDE
    else {
      if (!addFDE(*ent_region, pBackend, frag_list)) {
        m_fCanRecognizeAll = false;
        pArea.release(ent_region);
        break;
      }
    }
    p += len;
  }

  if (!m_fCanRecognizeAll) {
    pArea.release(region);
    deleteFragments(frag_list, pArea);
    return 0;
  }

  // append all CIE and FDE fragments to Layout after we successfully read
  // this eh_frame
  size_t section_size = 0;
  for (FragListType::iterator it = frag_list.begin();
         it != frag_list.end(); ++it)
    section_size += pLayout.appendFragment(**it, pSD, pSection.align());

  pArea.release(region);
  return section_size;
}

bool EhFrame::addCIE(MemoryRegion& pRegion,
                     const TargetLDBackend& pBackend,
                     FragListType& pFragList)
{
  ConstAddress cie_start = pRegion.start();
  ConstAddress cie_end = pRegion.end();
  ConstAddress p = cie_start;

  // skip the Length (4 byte) and CIE ID (4 byte) fields
  p += 8;

  // the version should be 1
  if (1 != *p) {
    debug(diag::debug_eh_unsupport) << "CIE version";
    return false;
  }
  ++p;

  // get the Augumentation String
  ConstAddress aug_str = p;
  ConstAddress aug_str_end = static_cast<ConstAddress>(
                               memchr(p, '\0', cie_end - p));

  // skip the Augumentation String field
  p = aug_str_end + 1;

  // skip the Code Alignment Factor
  if (!skipLEB128(&p, cie_end)) {
    debug(diag::debug_eh_unsupport) << "unrecognized Code Alignment Factor";
    return false;
  }
  // skip the Data Alignment Factor
  if (!skipLEB128(&p, cie_end)) {
    debug(diag::debug_eh_unsupport) << "unrecognized Data Alignment Factor";
    return false;
  }
  // skip the Return Address Register
  if (cie_end - p < 1) {
    debug(diag::debug_eh_unsupport) << "unrecognized Return Address Register";
    return false;
  }
  ++p;

  // the Augmentation String start with 'eh' is a CIE from gcc before 3.0,
  // in LSB Core Spec 3.0RC1. We do not support it.
  if (aug_str[0] == 'e' && aug_str[1] == 'h') {
    debug(diag::debug_eh_unsupport) << "augmentation string `eh`";
    return false;
  }

  // parse the Augmentation String to get the FDE encodeing if 'z' existed
  std::string aug_str_data;
  uint8_t fde_encoding = llvm::dwarf::DW_EH_PE_absptr;
  if (*aug_str == 'z') {

    aug_str_data += *aug_str;
    ++aug_str;

    // skip the Augumentation Data Length
    if (!skipLEB128(&p, cie_end)) {
      debug(diag::debug_eh_unsupport) <<
        "unrecognized Augmentation Data Length";
      return false;
    }

    while (aug_str != aug_str_end) {
      switch (*aug_str) {
        default:
          debug(diag::debug_eh_unsupport) << "augmentation string";
          return false;

        // LDSA encoding (1 byte)
        case 'L':
          if (cie_end - p < 1) {
            debug(diag::debug_eh_unsupport) << "augmentation string `L`";
            return false;
          }
          ++p;
          break;

        // Two arguments, the first one represents the encoding of the second
        // argument (1 byte). The second one is the address of personality
        // routine.
        case 'P': {
          // the first argument
          if (cie_end - p < 1) {
            debug(diag::debug_eh_unsupport) << "augmentation string `P`";
            return false;
          }
          uint8_t per_encode = *p;
          ++p;
          // get the length of the second argument
          uint32_t per_length = 0;
          if (0x60 == (per_encode & 0x60)) {
            debug(diag::debug_eh_unsupport) << "augmentation string `P`";
            return false;
          }
          switch (per_encode & 7) {
            default:
              debug(diag::debug_eh_unsupport) << "augmentation string `P`";
              return false;
            case llvm::dwarf::DW_EH_PE_udata2:
              per_length = 2;
              break;
            case llvm::dwarf::DW_EH_PE_udata4:
              per_length = 4;
              break;
            case llvm::dwarf::DW_EH_PE_udata8:
              per_length = 8;
              break;
            case llvm::dwarf::DW_EH_PE_absptr:
              per_length = pBackend.bitclass() / 8;
              break;
          }
          // skip the alignment
          if (llvm::dwarf::DW_EH_PE_aligned == (per_encode & 0xf0)) {
            uint32_t per_align = p - cie_end;
            per_align += per_length - 1;
            per_align &= ~(per_length -1);
            if (static_cast<uint32_t>(cie_end - p) < per_align) {
              debug(diag::debug_eh_unsupport) << "augmentation string `P`";
              return false;
            }
            p += per_align;
          }
          // skip the second argument
          if (static_cast<uint32_t>(cie_end - p) < per_length) {
            debug(diag::debug_eh_unsupport) << "augmentation string `P`";
            return false;
          }
          p += per_length;
        }
        break;

        // FDE encoding (1 byte)
        case 'R':
          if (cie_end - p < 1) {
            debug(diag::debug_eh_unsupport) << "augmentation string `R`";
            return false;
          }
          fde_encoding = *p;
          switch (fde_encoding & 7) {
            case llvm::dwarf::DW_EH_PE_udata2:
            case llvm::dwarf::DW_EH_PE_udata4:
            case llvm::dwarf::DW_EH_PE_udata8:
            case llvm::dwarf::DW_EH_PE_absptr:
              break;
            default:
              debug(diag::debug_eh_unsupport) << "FDE encoding";
              return false;
          }
          ++p;
          break;
      } // end switch
      aug_str_data += *aug_str;
      ++aug_str;
    } // end while
  }

  note(diag::note_eh_cie) << pRegion.size()
                          << aug_str_data
                          << (fde_encoding & 7);

  // create and push back the CIE entry
  CIE* entry = new CIE(pRegion, fde_encoding);
  m_CIEs.push_back(entry);
  pFragList.push_back(static_cast<llvm::MCFragment*>(entry));
  return true;
}

bool EhFrame::addFDE(MemoryRegion& pRegion,
                     const TargetLDBackend& pBackend,
                     FragListType& pFragList)
{
  ConstAddress fde_start = pRegion.start();
  ConstAddress fde_end = pRegion.end();
  ConstAddress p = fde_start;

  // skip the Length (4 byte) and CIE Pointer (4 byte) fields
  p += 8;

  // get the entry offset of the PC Begin
  if (fde_end - p < 1) {
    debug(diag::debug_eh_unsupport) << "FDE PC Begin";
    return false;
  }
  FDE::Offset pc_offset = static_cast<FDE::Offset>(p - fde_start);

  note(diag::note_eh_fde) << pRegion.size() << pc_offset;
  // create and push back the FDE entry
  FDE* entry = new FDE(pRegion, **(m_CIEs.end() -1), pc_offset);
  m_FDEs.push_back(entry);
  pFragList.push_back(static_cast<llvm::MCFragment*>(entry));
  return true;
}

uint32_t EhFrame::readVal(ConstAddress pAddr, bool pIsTargetLittleEndian)
{
  const uint32_t* p = reinterpret_cast<const uint32_t*>(pAddr);
  uint32_t val = *p;

  // byte swapping if the host and target have different endian
  if (llvm::sys::isLittleEndianHost() != pIsTargetLittleEndian)
    val = bswap32(val);
  return val;
}

bool EhFrame::skipLEB128(ConstAddress* pp, ConstAddress pend)
{
  for (ConstAddress p = *pp; p < pend; ++p) {
    if (0 == (*p & 0x80)) {
      *pp = p + 1;
      return true;
    }
  }
  return false;
}

void EhFrame::deleteFragments(FragListType& pList, MemoryArea& pArea)
{
  MCRegionFragment* frag = NULL;
  for (FragListType::iterator it = pList.begin(); it != pList.end(); ++it) {
    frag = static_cast<MCRegionFragment*>(*it);
    pArea.release(&(frag->getRegion()));
    delete *it;
  }
  pList.clear();
}

