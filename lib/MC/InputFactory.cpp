//===- InputFactory.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/InputFactory.h"
#include "mcld/MC/AttributeFactory.h"

using namespace mcld;

//===----------------------------------------------------------------------===//
// InputFactory
InputFactory::InputFactory(size_t pNum, AttributeFactory& pAttrFactory)
  : GCFactory<Input,0>(pNum), m_AttrFactory(pAttrFactory) {
}

InputFactory::~InputFactory()
{
}

Input* InputFactory::produce(llvm::StringRef pName,
                             const sys::fs::Path& pPath,
                             unsigned int pType,
                             off_t pFileOffset)
{
  mcld::Input* result = Alloc::allocate();
  new (result) mcld::Input(pName, pPath, m_AttrFactory.last(), pType, pFileOffset);
  return result;
}

