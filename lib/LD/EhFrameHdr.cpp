//===- EhFrameHdr.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrameHdr.h>

using namespace mcld;

//==========================
// EhFrameHdr
EhFrameHdr::EhFrameHdr(const EhFrame& pEhFrameData,
                       const LDSection& pEhFrameSect,
                       LDSection& pEhFrameHdrSect)
  : m_EhFrameData(pEhFrameData),
    m_EhFrameSect(pEhFrameSect),
    m_EhFrameHdrSect(pEhFrameHdrSect)
{
}

EhFrameHdr::~EhFrameHdr()
{
}

/// @ref lsb core generic 4.1
/// .eh_frame_hdr section format
/// uint8_t : version
/// uint8_t : eh_frame_ptr_enc
/// uint8_t : fde_count_enc
/// uint8_t : table_enc
/// uint32_t : eh_frame_ptr
/// uint32_t : fde_count
/// __________________________ when fde_count > 0
/// <uint32_t, uint32_t>+ : binary search table

/// sizeOutput - base on the fde count to size output
void EhFrameHdr::sizeOutput()
{
  size_t size = 12;
  if (m_EhFrameData.canRecognizeAllEhFrame()) {
    size_t fde_count = m_EhFrameData.getFDECount();
    size += 8 * fde_count;
  }
  m_EhFrameHdrSect.setSize(size);
}

