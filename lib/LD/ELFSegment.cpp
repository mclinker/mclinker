//===- ELFSegment.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFSegment.h>

using namespace mcld;

//==========================
// ELFSegment
ELFSegment::ELFSegment(uint32_t pType,
                       uint32_t pFlag,
                       uint64_t pOffset,
                       uint64_t pVaddr,
                       uint64_t pPaddr,
                       uint64_t pFilesz,
                       uint64_t pMemsz,
                       uint64_t pAlign,
                       uint64_t pMaxSectAlign)
  : m_Type(pType),
    m_Flag(pFlag),
    m_Offset(pOffset),
    m_Vaddr(pVaddr),
    m_Paddr(pPaddr),
    m_Filesz(pFilesz),
    m_Memsz(pMemsz),
    m_Align(pAlign),
    m_MaxSectionAlign(pMaxSectAlign) {
}

ELFSegment::~ELFSegment()
{
}
