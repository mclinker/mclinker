//===- UnaryOp.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_UNARY_OPERATOR_INTERFACE_H
#define MCLD_SCRIPT_UNARY_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/Operator.h>
#include <cstddef>

namespace mcld
{

class Operand;
class IntOperand;

/** \class UnaryOp
 *  \brief This class defines the interfaces to an unary operator token.
 */

template<Operator::Type TYPE>
class UnaryOp : public Operator
{
private:
  friend class Operator;

  UnaryOp()
    : Operator(Operator::Unary, TYPE), m_pOperand(NULL)
  {}

public:
  ~UnaryOp()
  {}

  IntOperand* eval();

  void appendOperand(Operand* pOperand)
  {
    m_pOperand = pOperand;
  }

private:
  Operand* m_pOperand;
};

template<>
IntOperand* UnaryOp<Operator::UNARY_PLUS>::eval();
template<>
IntOperand* UnaryOp<Operator::UNARY_MINUS>::eval();
template<>
IntOperand* UnaryOp<Operator::LOGICAL_NOT>::eval();
template<>
IntOperand* UnaryOp<Operator::BITWISE_NOT>::eval();

template<>
IntOperand* UnaryOp<Operator::ABSOLUTE>::eval();
template<>
IntOperand* UnaryOp<Operator::ADDR>::eval();
template<>
IntOperand* UnaryOp<Operator::ALIGNOF>::eval();
template<>
IntOperand* UnaryOp<Operator::DATA_SEGMENT_END>::eval();
template<>
IntOperand* UnaryOp<Operator::DEFINED>::eval();
template<>
IntOperand* UnaryOp<Operator::LENGTH>::eval();
template<>
IntOperand* UnaryOp<Operator::LOADADDR>::eval();
template<>
IntOperand* UnaryOp<Operator::NEXT>::eval();
template<>
IntOperand* UnaryOp<Operator::ORIGIN>::eval();
template<>
IntOperand* UnaryOp<Operator::SIZEOF>::eval();

} // namespace of mcld

#endif

