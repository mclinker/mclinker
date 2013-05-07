//===- TernaryOp.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TERNARY_OPERATOR_INTERFACE_H
#define MCLD_TERNARY_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/Operator.h>
#include <cassert>

namespace mcld
{

/** \class TernaryOP
 *  \brief This class defines the interfaces to an binary operator token.
 */

template<Operator::Type TYPE>
class TernaryOp : public Operator
{
private:
  friend class Operator;

  TernaryOp()
    : Operator(Operator::Ternary, TYPE), m_Size(0)
  {
    m_Operand[0] = m_Operand[1] = m_Operand[2] = 0;
  }

public:
  ~TernaryOp()
  {}

  Operator::ValueType eval();

  void appendOperand(Operator::ValueType pValue)
  {
    assert(m_Size < 3);
    m_Operand[m_Size++] = pValue;
    if (m_Size == 3)
      m_Size = 0;
  }

private:
  size_t m_Size;
  ValueType m_Operand[3];
};

template<>
Operator::ValueType TernaryOp<Operator::TERNARY_IF>::eval();

} // namespace of mcld

#endif
