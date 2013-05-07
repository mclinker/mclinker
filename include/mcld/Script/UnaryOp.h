//===- UnaryOp.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_UNARY_OPERATOR_INTERFACE_H
#define MCLD_UNARY_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/Operator.h>

namespace mcld
{

/** \class UnaryOp
 *  \brief This class defines the interfaces to an unary operator token.
 */

template<Operator::Type TYPE>
class UnaryOp : public Operator
{
private:
  friend class Operator;

  UnaryOp()
    : Operator(Operator::Unary, TYPE), m_Operand(0)
  {
  }

public:
  ~UnaryOp()
  {}

  Operator::ValueType eval();

  void appendOperand(Operator::ValueType pValue)
  {
    m_Operand = pValue;
  }

private:
  ValueType m_Operand;
};

template<>
Operator::ValueType UnaryOp<Operator::UNARY_PLUS>::eval();

template<>
Operator::ValueType  UnaryOp<Operator::UNARY_MINUS>::eval();

template<>
Operator::ValueType UnaryOp<Operator::LOGICAL_NOT>::eval();

template<>
Operator::ValueType UnaryOp<Operator::BITWISE_NOT>::eval();

} // namespace of mcld

#endif

