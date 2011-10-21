//===- MCLDInput.cpp ------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
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

