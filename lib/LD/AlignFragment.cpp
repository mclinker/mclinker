//===- AlignFragment.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/AlignFragment.h>

#include <mcld/LD/SectionData.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// AlignFragment
//===----------------------------------------------------------------------===//
AlignFragment::AlignFragment(unsigned int pAlignment,
                             int64_t pValue,
                             unsigned int pValueSize,
                             unsigned int pMaxBytesToEmit,
                             SectionData *pSD)
  : Fragment(Fragment::Alignment, pSD), m_Alignment(pAlignment),
    m_Value(pValue), m_ValueSize(pValueSize), m_MaxBytesToEmit(pMaxBytesToEmit),
    m_bEmitNops(false) {
}

