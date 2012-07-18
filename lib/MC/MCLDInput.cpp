//===- MCLDInput.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDAttribute.h"

using namespace mcld;

//==========================
// MCInput
Input::Input(llvm::StringRef pName, const AttributeProxy& pProxy)
 : MCLDFile(pName),
   m_pAttr(const_cast<Attribute*>(pProxy.attr())),
   m_bNeeded(false),
   m_fileOffset(0) {
}

Input::Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const AttributeProxy& pProxy,
        unsigned int pType,
        off_t pFileOffset)
  : MCLDFile(pName, pPath, pType),
    m_pAttr(const_cast<Attribute*>(pProxy.attr())),
    m_bNeeded(false),
    m_fileOffset(pFileOffset) {
}

Input::~Input()
{
  // do nothing. Attribute is deleted by AttributeFactory
}

