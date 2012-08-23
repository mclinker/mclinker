//===- MCLDInput.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDAttribute.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// MCInput
//===----------------------------------------------------------------------===//
Input::Input(llvm::StringRef pName, const AttributeProxy& pProxy)
  : MCLDFile(),
    m_Type(Unknown),
    m_Name(pName.data()),
    m_pAttr(const_cast<Attribute*>(pProxy.attr())),
    m_bNeeded(false),
    m_fileOffset(0),
    m_pMemArea(NULL) {
}

Input::Input(llvm::StringRef pName,
        const sys::fs::Path& pPath,
        const AttributeProxy& pProxy,
        unsigned int pType,
        off_t pFileOffset)
  : MCLDFile(pPath),
    m_Type(pType),
    m_Name(pName.data()),
    m_pAttr(const_cast<Attribute*>(pProxy.attr())),
    m_bNeeded(false),
    m_fileOffset(pFileOffset),
    m_pMemArea(NULL) {
}

Input::~Input()
{
  // do nothing.
  // Attribute is deleted by AttributeFactory
  // MemoryArea is deleted by MemoryAreaFactory
}

