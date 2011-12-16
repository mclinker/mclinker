//===- LDSection.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LDSection.h>
#include <llvm/MC/SectionKind.h>

using namespace mcld;

LDSection::LDSection(LDFileFormat::Kind pKind,
                     const std::string& pName,
                     uint64_t pSize,
                     uint64_t pOffset,
                     uint64_t pAddr,
                     uint32_t pFlag,
                     uint32_t pType)
  : llvm::MCSection(llvm::MCSection::SV_LDContext, llvm::SectionKind::getMetadata()),
    m_Kind(pKind),
    m_Name(pName),
    m_Size(pSize),
    m_Offset(pOffset),
    m_Addr(pAddr),
    m_Flag(pFlag),
    m_Type(pType) {
}

