//===- NullaryOp.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/NullaryOp.h>
#include <mcld/Script/Operand.h>
#include <mcld/Target/TargetLDBackend.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// NullaryOp
//===----------------------------------------------------------------------===//
template<>
IntOperand* NullaryOp<Operator::SIZEOF_HEADERS>::eval()
{
  IntOperand* res = result();
  res->setValue(backend().sectionStartOffset());
  return res;
}

template<>
IntOperand* NullaryOp<Operator::MAXPAGESIZE>::eval()
{
  IntOperand* res = result();
  res->setValue(backend().abiPageSize());
  return res;
}

template<>
IntOperand* NullaryOp<Operator::COMMONPAGESIZE>::eval()
{
  IntOperand* res = result();
  res->setValue(backend().commonPageSize());
  return res;
}
