//===- UnaryOp.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/UnaryOp.h>
#include <mcld/Script/Operand.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// UnaryOp
//===----------------------------------------------------------------------===//
template<>
IntOperand* UnaryOp<Operator::UNARY_PLUS>::eval()
{
  IntOperand* res = result();
  res->setValue(+ m_pOperand->value());
  return res;
}

template<>
IntOperand* UnaryOp<Operator::UNARY_MINUS>::eval()
{
  IntOperand* res = result();
  res->setValue(- m_pOperand->value());
  return res;
}

template<>
IntOperand* UnaryOp<Operator::LOGICAL_NOT>::eval()
{
  IntOperand* res = result();
  res->setValue(! m_pOperand->value());
  return res;
}

template<>
IntOperand* UnaryOp<Operator::BITWISE_NOT>::eval()
{
  IntOperand* res = result();
  res->setValue(~ m_pOperand->value());
  return res;
}


