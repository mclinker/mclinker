//===- BinaryOp.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_BINARY_OPERATOR_INTERFACE_H
#define MCLD_BINARY_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/Operator.h>
#include <cassert>

namespace mcld
{

/** \class BinaryOP
 *  \brief This class defines the interfaces to an binary operator token.
 */

template<Operator::Type TYPE>
class BinaryOp : public Operator
{
private:
  friend class Operator;

  BinaryOp()
    : Operator(Operator::Binary, TYPE), m_Size(0)
  {
    m_Operand[0] = m_Operand[1] = 0;
  }

public:
  ~BinaryOp()
  {}

  Operator::ValueType eval();

  void appendOperand(Operator::ValueType pValue)
  {
    assert(m_Size < 2);
    m_Operand[m_Size++] = pValue;
    if (m_Size == 2)
      m_Size = 0;
  }

private:
  size_t m_Size;
  ValueType m_Operand[2];
};

template<>
Operator::ValueType BinaryOp<Operator::MUL>::eval();
template<>
Operator::ValueType BinaryOp<Operator::DIV>::eval();
template<>
Operator::ValueType BinaryOp<Operator::MOD>::eval();
template<>
Operator::ValueType BinaryOp<Operator::ADD>::eval();
template<>
Operator::ValueType BinaryOp<Operator::SUB>::eval();
template<>
Operator::ValueType BinaryOp<Operator::LSHIFT>::eval();
template<>
Operator::ValueType BinaryOp<Operator::RSHIFT>::eval();
template<>
Operator::ValueType BinaryOp<Operator::LT>::eval();
template<>
Operator::ValueType BinaryOp<Operator::LE>::eval();
template<>
Operator::ValueType BinaryOp<Operator::GT>::eval();
template<>
Operator::ValueType BinaryOp<Operator::GE>::eval();
template<>
Operator::ValueType BinaryOp<Operator::EQ>::eval();
template<>
Operator::ValueType BinaryOp<Operator::NE>::eval();
template<>
Operator::ValueType BinaryOp<Operator::BITWISE_AND>::eval();
template<>
Operator::ValueType BinaryOp<Operator::BITWISE_XOR>::eval();
template<>
Operator::ValueType BinaryOp<Operator::BITWISE_OR>::eval();
template<>
Operator::ValueType BinaryOp<Operator::LOGICAL_AND>::eval();
template<>
Operator::ValueType BinaryOp<Operator::LOGICAL_OR>::eval();
template<>
Operator::ValueType BinaryOp<Operator::ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::ADD_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::SUB_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::MUL_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::DIV_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::AND_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::OR_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::LS_ASSIGN>::eval();
template<>
Operator::ValueType BinaryOp<Operator::RS_ASSIGN>::eval();

} // namespace of mcld

#endif

