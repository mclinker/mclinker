//===- InputFactory.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/MC/InputFactory.h>
#include <mcld/MC/AttributeFactory.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// InputFactory
//===----------------------------------------------------------------------===//
InputFactory::InputFactory(size_t pNum)
  : GCFactory<Input,0>(pNum) {

  m_pAttrFactory = new AttributeFactory(16, m_Predefined);
  m_pLast = new AttributeProxy(*m_pAttrFactory, m_Predefined, m_Constraint);
}

InputFactory::~InputFactory()
{
  delete m_pAttrFactory;
  delete m_pLast;
}

Input* InputFactory::produce(llvm::StringRef pName,
                             const sys::fs::Path& pPath,
                             unsigned int pType,
                             off_t pFileOffset)
{
  mcld::Input* result = Alloc::allocate();
  new (result) mcld::Input(pName, pPath, *m_pLast, pType, pFileOffset);
  return result;
}

bool InputFactory::checkAttributes() const
{
  mcld::AttributeFactory::const_iterator attr, attrEnd = m_pAttrFactory->end();
  for (attr=m_pAttrFactory->begin(); attr!=attrEnd; ++attr) {
    if (!m_Constraint.isLegal(**attr)) {
      return false;
    }
  }
  return true;
}

