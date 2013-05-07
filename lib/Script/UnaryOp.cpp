//===- UnaryOp.cpp --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/UnaryOp.h>
#include <mcld/Support/raw_ostream.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// UnaryOp
//===----------------------------------------------------------------------===//
template<>
Operator::ValueType UnaryOp<Operator::UNARY_PLUS>::eval()
{
  return + m_Operand;
}

template<>
Operator::ValueType UnaryOp<Operator::UNARY_MINUS>::eval()
{
  return - m_Operand;
}

template<>
Operator::ValueType UnaryOp<Operator::LOGICAL_NOT>::eval()
{
  return ! m_Operand;
}

template<>
Operator::ValueType UnaryOp<Operator::BITWISE_NOT>::eval()
{
  return ~ m_Operand;
}


