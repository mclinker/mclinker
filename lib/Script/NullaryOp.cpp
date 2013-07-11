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

using namespace mcld;
//===----------------------------------------------------------------------===//
// NullaryOp
//===----------------------------------------------------------------------===//
template<>
IntOperand* NullaryOp<Operator::SIZEOF_HEADERS>::eval()
{
  // TODO
  assert(0);
  return result();
}

template<>
IntOperand* NullaryOp<Operator::MAXPAGESIZE>::eval()
{
  // TODO
  assert(0);
  return result();
}

template<>
IntOperand* NullaryOp<Operator::COMMONPAGESIZE>::eval()
{
  // TODO
  assert(0);
  return result();
}
