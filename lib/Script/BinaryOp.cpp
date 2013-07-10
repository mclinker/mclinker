//===- BinaryOp.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/BinaryOp.h>
#include <mcld/Script/Operand.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// BinaryOp
//===----------------------------------------------------------------------===//
template<>
IntOperand* BinaryOp<Operator::MUL>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() * m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::DIV>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() / m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::MOD>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() % m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::ADD>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() + m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::SUB>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() - m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LSHIFT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() << m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::RSHIFT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() >> m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() < m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LE>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() <= m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::GT>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() > m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::GE>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() >= m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::EQ>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() == m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::NE>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() != m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::BITWISE_AND>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() & m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::BITWISE_XOR>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() ^ m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::BITWISE_OR>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() | m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LOGICAL_AND>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() && m_pOperand[1]->value());
  return res;
}

template<>
IntOperand* BinaryOp<Operator::LOGICAL_OR>::eval()
{
  IntOperand* res = result();
  res->setValue(m_pOperand[0]->value() || m_pOperand[1]->value());
  return res;
}
