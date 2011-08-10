/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDInput.h>

using namespace mcld;

//==========================
// MCInput
Input::Input()
 : MCLDFile(), m_pAttr(0){
}

Input::Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const MCLDAttribute& pAttr,
        unsigned int pType)
  : MCLDFile(pName, pPath, pType),
    m_pAttr(const_cast<MCLDAttribute*>(&pAttr)) {
}

Input::~Input()
{
}

