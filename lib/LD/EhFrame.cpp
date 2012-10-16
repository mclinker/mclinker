//===- EhFrame.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <llvm/Support/Host.h>
#include <llvm/Support/Dwarf.h>
#include <mcld/Fragment/FillFragment.h>
#include <mcld/Fragment/FragmentLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/MemoryRegion.h>

using namespace mcld;

//==========================
// EhFrame
EhFrame::EhFrame(bool isLittleEndianTarget)
 : m_fTreatAsRegularSection(false) {
  if (llvm::sys::isLittleEndianHost() == isLittleEndianTarget)
    m_pReadVal = new ReadVal<false>();
  else
    m_pReadVal = new ReadVal<true>();
}

EhFrame::~EhFrame()
{
  if (NULL != m_pReadVal)
    delete m_pReadVal;
}

size_t EhFrame::read(Layout& pLayout,
                     Input& pInput,
                     LDSection& pSection,
                     unsigned int pBitclass)
{
  // prototype of action function
  typedef State (EhFrame::*ActionFuncType)(PackageType&);

  // Table of action functions
  static const ActionFuncType ActionFuncs[] = {
    &EhFrame::start,
    &EhFrame::checkFirstCIE,
    &EhFrame::parseEntry,
    &EhFrame::addCIE,
    &EhFrame::addFDE,
    &EhFrame::fail,
    &EhFrame::success,
    &EhFrame::readRegular
  };

  // setup the Package
  Package pkg(pLayout, pInput, pSection, pBitclass);
  State state = Start;

  // parse the eh_frame
  while (Stop != state) {
    state = (this->*ActionFuncs[state])(pkg);
  }
  size_t section_size = pkg.sectSize;
  return section_size;
}

EhFrame::State EhFrame::start(PackageType& pPkg)
{
  // get MemoryRegion of the section
  MemoryRegion* region = pPkg.input.memArea()->request(
         pPkg.input.fileOffset() + pPkg.section.offset(), pPkg.section.size());
  pPkg.sectionRegion = region;

  // an empty .eh_frame, handle as regular section
  if (NULL == region)
    return ReadRegular;

  // we fail to parse some eh_frame prior to this one, we should then treat all
  // the other one as regular sections
  if (treatAsRegularSection())
    return ReadRegular;

  // initialize the Package
  pPkg.sectCur = region->start();
  pPkg.sectStart = region->start();
  pPkg.sectEnd = region->end();
  return CheckFirstCIE;
}

EhFrame::State EhFrame::checkFirstCIE(PackageType& pPkg)
{
  // read the Length field of the first entry
  uint32_t len = (*m_pReadVal)(pPkg.sectCur);

  // This CIE is a terminator if the Length field is 0, handle it as a regular
  // section
  if (0x0 == len)
    return ReadRegular;

  // not support 64 bit eh_frame
  if (0xffffffff == len)
    return Fail;

  // check the ID field, the first entry should be CIE
  if (0x0 != (*m_pReadVal)(pPkg.sectCur + 4))
    return Fail;

  return ParseEntry;
}

EhFrame::State EhFrame::parseEntry(PackageType& pPkg)
{
  ConstAddress handler = pPkg.sectCur;

  // end of section
  if (handler >= pPkg.sectEnd)
    return Success;

  // invalid length field size
  if (pPkg.sectEnd - handler < 4)
    return Fail;

  // read the Length field
  uint32_t len = (*m_pReadVal)(handler);
  handler += 4;

  uint32_t ent_offset = 0;
  MemoryRegion* ent_region = NULL;

  if (0x0 == len) {
    // the zero length entry should be the end of the section
    if (handler < pPkg.sectEnd)
      return Fail;

    // push back fragment of this terminate entry
    ent_offset = static_cast<uint32_t>(handler - pPkg.sectStart - 4);
    ent_region = pPkg.input.memArea()->request(
        pPkg.input.fileOffset() + pPkg.section.offset() + ent_offset, len + 4);
    pPkg.fragList.push_back(new RegionFragment(*ent_region));
    return Success;
  }

  // not support 64 bit eh_frame
  if (0xffffffff == len) {
    return Fail;
  }

  // invalid size of ID field
  if (pPkg.sectEnd - handler < 4) {
    return Fail;
  }

  // compute the section offset of this entry
  ent_offset = static_cast<uint32_t>(handler - pPkg.sectStart - 4);

  // get the MemoryRegion for this entry
  ent_region = pPkg.input.memArea()->request(
        pPkg.input.fileOffset() + pPkg.section.offset() + ent_offset, len + 4);

  // put entry's MemoryRegion into Package
  pPkg.entryRegion = ent_region;

  // read ID field
  uint32_t id = (*m_pReadVal)(handler);

  // set sectCur to the next entry
  handler += len;
  pPkg.sectCur = handler;

  if (0 == id) {
    return AddCIE;
  }
  return AddFDE;
}

EhFrame::State EhFrame::addCIE(PackageType& pPkg)
{
  assert(pPkg.entryRegion != NULL);
  ConstAddress cie_end = pPkg.entryRegion->end();
  ConstAddress handler = pPkg.entryRegion->start();

  // skip the Length (4 byte) and CIE ID (4 byte) fields
  handler += 8;

  // the version should be 1
  if (1 != *handler) {
    return Fail;
  }
  ++handler;

  // get the Augumentation String
  ConstAddress aug_str = handler;
  ConstAddress aug_str_end = static_cast<ConstAddress>(
                               memchr(handler, '\0', cie_end - handler));

  // skip the Augumentation String field
  handler = aug_str_end + 1;

  // skip the Code Alignment Factor
  if (!skipLEB128(&handler, cie_end)) {
    return Fail;
  }
  // skip the Data Alignment Factor
  if (!skipLEB128(&handler, cie_end)) {
    return Fail;
  }
  // skip the Return Address Register
  if (cie_end - handler < 1) {
    return Fail;
  }
  ++handler;

  // the Augmentation String start with 'eh' is a CIE from gcc before 3.0,
  // in LSB Core Spec 3.0RC1. We do not support it.
  if (aug_str[0] == 'e' && aug_str[1] == 'h') {
    return Fail;
  }

  // parse the Augmentation String to get the FDE encodeing if 'z' existed
  std::string aug_str_data;
  uint8_t fde_encoding = llvm::dwarf::DW_EH_PE_absptr;
  if (*aug_str == 'z') {

    aug_str_data += *aug_str;
    ++aug_str;

    // skip the Augumentation Data Length
    if (!skipLEB128(&handler, cie_end)) {
      return Fail;
    }

    // parse the Augmentation String
    while (aug_str != aug_str_end) {
      switch (*aug_str) {
        default:
          return Fail;

        // LDSA encoding (1 byte)
        case 'L':
          if (cie_end - handler < 1) {
            return Fail;
          }
          ++handler;
          break;

        // Two arguments, the first one represents the encoding of the second
        // argument (1 byte). The second one is the address of personality
        // routine.
        case 'P': {
          // the first argument
          if (cie_end - handler < 1) {
            return Fail;
          }
          uint8_t per_encode = *handler;
          ++handler;
          // get the length of the second argument
          uint32_t per_length = 0;
          if (0x60 == (per_encode & 0x60)) {
            return Fail;
          }
          switch (per_encode & 7) {
            default:
              return Fail;
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
              per_length = pPkg.bitclass / 8;
              break;
          }
          // skip the alignment
          if (llvm::dwarf::DW_EH_PE_aligned == (per_encode & 0xf0)) {
            uint32_t per_align = handler - cie_end;
            per_align += per_length - 1;
            per_align &= ~(per_length -1);
            if (static_cast<uint32_t>(cie_end - handler) < per_align) {
              return Fail;
            }
            handler += per_align;
          }
          // skip the second argument
          if (static_cast<uint32_t>(cie_end - handler) < per_length) {
            return Fail;
          }
          handler += per_length;
        }
        break;

        // FDE encoding (1 byte)
        case 'R':
          if (cie_end - handler < 1) {
            return Fail;
          }
          fde_encoding = *handler;
          switch (fde_encoding & 7) {
            case llvm::dwarf::DW_EH_PE_udata2:
            case llvm::dwarf::DW_EH_PE_udata4:
            case llvm::dwarf::DW_EH_PE_udata8:
            case llvm::dwarf::DW_EH_PE_absptr:
              break;
            default:
              return Fail;
          }
          ++handler;
          break;
      } // end switch
      aug_str_data += *aug_str;
      ++aug_str;
    } // end while
  }

  note(diag::note_eh_cie) << pPkg.entryRegion->size()
                          << aug_str_data
                          << (fde_encoding & 7);

  // create and push back the CIE entry
  CIE* entry = new CIE(*(pPkg.entryRegion), fde_encoding);
  m_CIEs.push_back(entry);
  pPkg.fragList.push_back(static_cast<Fragment*>(entry));
  return ParseEntry;
}

EhFrame::State EhFrame::addFDE(PackageType& pPkg)
{
  assert(pPkg.sectionRegion != NULL);
  ConstAddress fde_start = pPkg.entryRegion->start();
  ConstAddress fde_end = pPkg.entryRegion->end();
  ConstAddress handler = pPkg.entryRegion->start();

  // skip the Length (4 byte) and CIE Pointer (4 byte) fields
  handler += 8;

  // get the entry offset of the PC Begin
  if (fde_end - handler < 1) {
    return Fail;
  }
  FDE::Offset pc_offset = static_cast<FDE::Offset>(handler - fde_start);

  note(diag::note_eh_fde) << pPkg.entryRegion->size() << pc_offset;
  // create and push back the FDE entry
  FDE* entry = new FDE(*pPkg.entryRegion, **(m_CIEs.end() -1), pc_offset);
  m_FDEs.push_back(entry);
  pPkg.fragList.push_back(static_cast<Fragment*>(entry));
  return ParseEntry;
}

EhFrame::State EhFrame::fail(PackageType& pPkg)
{
  // fail to recognize thie eh_frame, which means that we don't need to parse
  // the rest of the eh_frame in other inputs because we end up has no ability
  // to generate the binary search table in eh_frame_hdr
  m_fTreatAsRegularSection = true;
  m_CIEs.clear();
  m_FDEs.clear();

  // relese the requested MemoryRegion for entry
  pPkg.input.memArea()->release(pPkg.entryRegion);

  // release the MemoryRegion and delete Fragments in FragList
  RegionFragment* frag = NULL;
  FragListType::iterator it = pPkg.fragList.begin();
  FragListType::iterator end = pPkg.fragList.end();
  for (; it != end; ++it) {
    frag = static_cast<RegionFragment*>(*it);
    pPkg.input.memArea()->release(&(frag->getRegion()));
    delete *it;
  }
  pPkg.fragList.clear();

  // output debug message
  debug(diag::debug_eh_unsupport) << pPkg.input.name();

  return ReadRegular;
}

EhFrame::State EhFrame::success(PackageType& pPkg)
{
  // append all CIE and FDE fragments to Layout after we successfully read
  // this eh_frame
  SectionData* sect_data = pPkg.section.getSectionData();
  assert(NULL != sect_data);
  FragListType::iterator it = pPkg.fragList.begin();
  FragListType::iterator end = pPkg.fragList.end();
  size_t size = 0;
  for (; it != end; ++it)
     size += pPkg.layout.appendFragment(**it, *sect_data, pPkg.section.align());
  pPkg.sectSize = size;

  // release the requested MemoryRegion for whole section
  pPkg.input.memArea()->release(pPkg.sectionRegion);
  return Stop;
}

EhFrame::State EhFrame::readRegular(PackageType& pPkg)
{
  // handle this eh_frame as a regular section
  MemoryRegion* region = pPkg.sectionRegion;
  Fragment* frag = NULL;
  if (NULL == region) {
    // If the input section's size is zero, we got a NULL region.
    // use a virtual fill fragment
    frag = new FillFragment(0x0, 0, 0);
  }
  else
    frag = new RegionFragment(*region);
  SectionData* sect_data = pPkg.section.getSectionData();
  assert(NULL != sect_data);
  pPkg.sectSize = pPkg.layout.appendFragment(
                                      *frag, *sect_data, pPkg.section.align());
  return Stop;
}

bool EhFrame::skipLEB128(ConstAddress* pp, ConstAddress pend)
{
  for (ConstAddress p = *pp; p < pend; ++p) {
    if (0x0 == (*p & 0x80)) {
      *pp = p + 1;
      return true;
    }
  }
  return false;
}

