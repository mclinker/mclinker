//===- EhFrameHdr.tcc -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <vector>

using namespace mcld;
using namespace llvm::dwarf;

/// emitOutput - write out eh_frame_hdr
template<size_t size>
void EhFrameHdr::emitOutput(Output& pOutput, MCLinker& pLinker)
{
  MemoryRegion* ehframe_region =
    pOutput.memArea()->request(m_EhFrameSect.offset(), m_EhFrameSect.size());

  MemoryRegion* ehframehdr_region =
    pOutput.memArea()->request(m_EhFrameHdrSect.offset(),
                               m_EhFrameHdrSect.size());

  uint8_t* data = (uint8_t*)ehframehdr_region->start();
  // version
  data[0] = 1;
  // eh_frame_ptr_enc
  data[1] = DW_EH_PE_pcrel | DW_EH_PE_sdata4;

  // eh_frame_ptr
  uint32_t* eh_frame_ptr = (uint32_t*)(data + 4);
  *eh_frame_ptr = m_EhFrameSect.addr() - (m_EhFrameHdrSect.addr() + 4);

  // fde_count
  uint32_t* fde_count = (uint32_t*)(data + 8);
  *fde_count = m_EhFrameData.getFDECount();

  if (m_EhFrameData.getFDECount() != 0 &&
      m_EhFrameData.canRecognizeAllEhFrame()) {
    // fde_count_enc
    data[2] = DW_EH_PE_udata4;
    // table_enc
    data[3] = DW_EH_PE_datarel | DW_EH_PE_sdata4;

    // prepare the binary search table
    typedef std::vector<typename BSTEntry<size>::EntryType> SearchTableType;
    SearchTableType search_table;
    EhFrame::const_fde_iterator fde = m_EhFrameData.fde_begin(),
                                fde_end = m_EhFrameData.fde_end();
    for(; fde != fde_end; ++fde) {
      assert(*fde != NULL);
      typename SizeTraits<size>::Offset offset;
      typename SizeTraits<size>::Address fde_pc;
      typename SizeTraits<size>::Address fde_addr;
      offset = pLinker.getLayout().getOutputOffset(**fde);
      fde_pc = getFDEPC<size>(**fde, offset, *ehframe_region);
      fde_addr = m_EhFrameSect.addr() + offset;
      search_table.push_back(std::make_pair(fde_pc, fde_addr));
    }

    std::sort(search_table.begin(), search_table.end(), BSTEntryCompare<size>());

    // write out the binary search table
    uint32_t* bst = (uint32_t*)(data + 12);
    typename SearchTableType::const_iterator entry = search_table.begin(),
                                             entry_end = search_table.end();
    for (size_t id = 0; entry != entry_end; ++entry) {
      bst[id++] = (*entry).first - m_EhFrameHdrSect.addr();
      bst[id++] = (*entry).second - m_EhFrameHdrSect.addr();
    }
  } else {
    // fde_count_enc
    data[2] = DW_EH_PE_omit;
    // table_enc
    data[3] = DW_EH_PE_omit;
  }

  pOutput.memArea()->release(ehframe_region);
  pOutput.memArea()->release(ehframehdr_region);
}

/// getFDEPC - return the address of FDE's pc
/// @ref binutils gold: ehframe.cc:222
template<size_t size>
typename SizeTraits<size>::Address
EhFrameHdr::getFDEPC(const FDE& pFDE,
                     typename SizeTraits<size>::Offset pOffset,
                     const MemoryRegion& pEhFrameRegion)
{
  uint8_t fde_encoding = pFDE.getCIE().getFDEEncode();
  unsigned int eh_value = fde_encoding & 0x7;

  // check the size to read in
  if (eh_value == llvm::dwarf::DW_EH_PE_absptr) {
    if (size == 32)
      eh_value = DW_EH_PE_udata4;
    else if (size == 64)
      eh_value = DW_EH_PE_udata8;
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

  typename SizeTraits<size>::Address pc = 0x0;
  const uint8_t* offset = (const uint8_t*) pEhFrameRegion.start() +
                          pOffset +
                          pFDE.getPCBeginOffset();
  std::memcpy(&pc, offset, pc_size);

  // adjust the signed value
  bool is_signed = (fde_encoding & llvm::dwarf::DW_EH_PE_signed) != 0x0;
  switch (eh_value) {
    case DW_EH_PE_udata2:
      if (is_signed)
        pc = (pc ^ 0x8000) - 0x8000;
      break;
    case DW_EH_PE_udata4:
      if (is_signed && size > 32)
        pc = (pc ^ 0x80000000) - 0x80000000;
      break;
    case DW_EH_PE_udata8:
      break;
    default:
      // TODO
      break;
  }

  // handle eh application
  switch (fde_encoding & 0x70)
  {
    case DW_EH_PE_absptr:
      break;
    case DW_EH_PE_pcrel:
      pc += m_EhFrameSect.addr() +
            pOffset +
            pFDE.getPCBeginOffset();
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

