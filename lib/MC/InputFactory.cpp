/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/AttributeFactory.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// InputFactory
InputFactory::InputFactory(size_t pNum, AttributeFactory& pAttrFactory)
  : GCFactory<Input,0>(pNum), m_AttrFactory(pAttrFactory) {
}

InputFactory::~InputFactory()
{
}

Input* InputFactory::produce(llvm::StringRef pName)
{
  mcld::Input* result = Alloc::allocate();
  new (result) mcld::Input(pName, m_AttrFactory.last());
  return result;
}

Input* InputFactory::produce(llvm::StringRef pName,
                             const sys::fs::Path& pPath,
                             unsigned int pType)
{
  mcld::Input* result = Alloc::allocate();
  new (result) mcld::Input(pName, pPath, m_AttrFactory.last(), pType);
  return result;
}

