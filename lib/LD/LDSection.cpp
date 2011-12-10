/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com> (owner)                                  *
 ****************************************************************************/
#include <mcld/LD/LDSection.h>

using namespace mcld;

LDSection::LDSection(llvm::SectionKind pKind,
                     const llvm::StringRef& pName,
                     uint64_t pSize,
                     uint64_t pOffset,
                     uint64_t pAddr,
                     uint32_t pFlag,
                     uint32_t pType)
  : llvm::MCSection(llvm::MCSection::SV_LDContext, pKind),
    m_Name(pName),
    m_Size(pSize),
    m_Offset(pOffset),
    m_Addr(pAddr),
    m_Flag(pFlag),
    m_Type(pType) {
}

