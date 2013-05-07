//===- BinaryOp.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/BinaryOp.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// BinaryOp
//===----------------------------------------------------------------------===//
template<>
Operator::ValueType BinaryOp<Operator::MUL>::eval()
{
  return m_Operand[0] * m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::DIV>::eval()
{
  return m_Operand[0] / m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::MOD>::eval()
{
  return m_Operand[0] % m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::ADD>::eval()
{
  return m_Operand[0] + m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::SUB>::eval()
{
  return m_Operand[0] - m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::LSHIFT>::eval()
{
  return m_Operand[0] << m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::RSHIFT>::eval()
{
  return m_Operand[0] >> m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::LT>::eval()
{
  return m_Operand[0] < m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::LE>::eval()
{
  return m_Operand[0] <= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::GT>::eval()
{
  return m_Operand[0] > m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::GE>::eval()
{
  return m_Operand[0] >= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::EQ>::eval()
{
  return m_Operand[0] == m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::NE>::eval()
{
  return m_Operand[0] != m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::BITWISE_AND>::eval()
{
  return m_Operand[0] & m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::BITWISE_XOR>::eval()
{
  return m_Operand[0] ^ m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::BITWISE_OR>::eval()
{
  return m_Operand[0] | m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::LOGICAL_AND>::eval()
{
  return m_Operand[0] && m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::LOGICAL_OR>::eval()
{
  return m_Operand[0] || m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::ASSIGN>::eval()
{
  return m_Operand[0] = m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::ADD_ASSIGN>::eval()
{
  return m_Operand[0] += m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::SUB_ASSIGN>::eval()
{
  return m_Operand[0] -= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::MUL_ASSIGN>::eval()
{
  return m_Operand[0] *= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::DIV_ASSIGN>::eval()
{
  return m_Operand[0] /= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::AND_ASSIGN>::eval()
{
  return m_Operand[0] &= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::OR_ASSIGN>::eval()
{
  return m_Operand[0] |= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::LS_ASSIGN>::eval()
{
  return m_Operand[0] <<= m_Operand[1];
}

template<>
Operator::ValueType BinaryOp<Operator::RS_ASSIGN>::eval()
{
  return m_Operand[0] >>= m_Operand[1];
}

