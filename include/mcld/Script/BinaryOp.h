//===- BinaryOp.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_BINARY_OPERATOR_INTERFACE_H
#define MCLD_SCRIPT_BINARY_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/Operator.h>
#include <cstddef>

namespace mcld
{

class Operand;
class IntOperand;
class Module;
class TargetLDBackend;

/** \class BinaryOP
 *  \brief This class defines the interfaces to an binary operator token.
 */

template<Operator::Type TYPE>
class BinaryOp : public Operator
{
private:
  friend class Operator;

  BinaryOp(const Module& pModule, const TargetLDBackend& pBackend)
    : Operator(pModule, pBackend, Operator::BINARY, TYPE), m_Size(0)
  {
    m_pOperand[0] = m_pOperand[1] = NULL;
  }

public:
  ~BinaryOp()
  {}

  IntOperand* eval();

  void appendOperand(Operand* pOperand)
  {
    m_pOperand[m_Size++] = pOperand;
    if (m_Size == 2)
      m_Size = 0;
  }

private:
  size_t m_Size;
  Operand* m_pOperand[2];
};

template<>
IntOperand* BinaryOp<Operator::MUL>::eval();
template<>
IntOperand* BinaryOp<Operator::DIV>::eval();
template<>
IntOperand* BinaryOp<Operator::MOD>::eval();
template<>
IntOperand* BinaryOp<Operator::ADD>::eval();
template<>
IntOperand* BinaryOp<Operator::SUB>::eval();
template<>
IntOperand* BinaryOp<Operator::LSHIFT>::eval();
template<>
IntOperand* BinaryOp<Operator::RSHIFT>::eval();
template<>
IntOperand* BinaryOp<Operator::LT>::eval();
template<>
IntOperand* BinaryOp<Operator::LE>::eval();
template<>
IntOperand* BinaryOp<Operator::GT>::eval();
template<>
IntOperand* BinaryOp<Operator::GE>::eval();
template<>
IntOperand* BinaryOp<Operator::EQ>::eval();
template<>
IntOperand* BinaryOp<Operator::NE>::eval();
template<>
IntOperand* BinaryOp<Operator::BITWISE_AND>::eval();
template<>
IntOperand* BinaryOp<Operator::BITWISE_XOR>::eval();
template<>
IntOperand* BinaryOp<Operator::BITWISE_OR>::eval();
template<>
IntOperand* BinaryOp<Operator::LOGICAL_AND>::eval();
template<>
IntOperand* BinaryOp<Operator::LOGICAL_OR>::eval();

template<>
IntOperand* BinaryOp<Operator::ALIGN>::eval();
template<>
IntOperand* BinaryOp<Operator::DATA_SEGMENT_RELRO_END>::eval();
template<>
IntOperand* BinaryOp<Operator::MAX>::eval();
template<>
IntOperand* BinaryOp<Operator::MIN>::eval();
template<>
IntOperand* BinaryOp<Operator::SEGMENT_START>::eval();

} // namespace of mcld

#endif

