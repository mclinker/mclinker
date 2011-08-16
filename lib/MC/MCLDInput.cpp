/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDAttribute.h>

using namespace mcld;

//==========================
// MCInput
Input::Input(llvm::StringRef pName, const AttributeProxy& pProxy)
 : MCLDFile(pName),
   m_pAttr(const_cast<Attribute*>(pProxy.attr())) {
}

Input::Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const AttributeProxy& pProxy,
        unsigned int pType)
  : MCLDFile(pName, pPath, pType),
    m_pAttr(const_cast<Attribute*>(pProxy.attr())) {
}

Input::~Input()
{
  // do nothing. Attribute is deleted by AttributeFactory
}

