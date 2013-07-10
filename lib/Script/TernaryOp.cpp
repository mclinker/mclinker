//===- TernaryOp.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/TernaryOp.h>
#include <mcld/Script/Operand.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// TernaryOp
//===----------------------------------------------------------------------===//
template<>
IntOperand* TernaryOp<Operator::TERNARY_IF>::eval()
{
  IntOperand* res = result();
  if (m_pOperand[0]->value())
    res->setValue(m_pOperand[1]->value());
  else
    res->setValue(m_pOperand[2]->value());
  return res;
}

