//===- TernaryOp.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/TernaryOp.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// TernaryOp
//===----------------------------------------------------------------------===//
template<>
Operator::ValueType TernaryOp<Operator::TERNARY_IF>::eval()
{
  return m_Operand[0] ? m_Operand[1] : m_Operand[2];
}

