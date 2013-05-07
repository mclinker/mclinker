//===- Operator.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OPERATOR_INTERFACE_H
#define MCLD_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ExprToken.h>
#include <llvm/Support/DataTypes.h>
#include <vector>
#include <cassert>

namespace mcld
{

/** \class Operator
 *  \brief This class defines the interfaces to an operator token.
 */

class Operator : public ExprToken
{
public:
  typedef uint64_t ValueType;

  enum Arity {
    Unary,
    Binary,
    Ternary
  };

  enum Type {
    UNARY_PLUS  = 0,
    UNARY_MINUS = 1,
    LOGICAL_NOT = 2,
    BITWISE_NOT = 3,
    MUL         = 4,
    DIV         = 5,
    MOD         = 6,
    ADD         = 7,
    SUB         = 8,
    LSHIFT      = 9,
    RSHIFT      = 10,
    LT          = 11,
    LE          = 12,
    GT          = 13,
    GE          = 14,
    EQ          = 15,
    NE          = 16,
    BITWISE_AND = 17,
    BITWISE_XOR = 18,
    BITWISE_OR  = 19,
    LOGICAL_AND = 20,
    LOGICAL_OR  = 21,
    TERNARY_IF  = 22,
    ASSIGN      = 23,
    ADD_ASSIGN  = 24,
    SUB_ASSIGN  = 25,
    MUL_ASSIGN  = 26,
    DIV_ASSIGN  = 27,
    AND_ASSIGN  = 28,
    OR_ASSIGN   = 29,
    LS_ASSIGN   = 30,
    RS_ASSIGN   = 31
  };

  static const char* OpNames[];

protected:
  Operator(Arity pArity, Type pType)
    : ExprToken(ExprToken::Op), m_Arity(pArity), m_Type(pType)
  {}

public:
  virtual ~Operator()
  {}

  Arity arity() const { return m_Arity; }

  Type type() const { return m_Type; }

  virtual void dump() const;

  virtual ValueType eval() = 0;

  virtual void appendOperand(ValueType pValue) = 0;

  static bool classof(const ExprToken* pToken)
  {
    return pToken->kind() == ExprToken::Op;
  }

  template<Operator::Type TYPE>
  static Operator& create();

private:
  Arity m_Arity;
  Type m_Type;
};

/* Unary operator */
template<>
Operator& Operator::create<Operator::UNARY_PLUS>();
template<>
Operator& Operator::create<Operator::UNARY_MINUS>();
template<>
Operator& Operator::create<Operator::LOGICAL_NOT>();
template<>
Operator& Operator::create<Operator::BITWISE_NOT>();

/* Binary operator */
template<>
Operator& Operator::create<Operator::MUL>();
template<>
Operator& Operator::create<Operator::DIV>();
template<>
Operator& Operator::create<Operator::MOD>();
template<>
Operator& Operator::create<Operator::ADD>();
template<>
Operator& Operator::create<Operator::SUB>();
template<>
Operator& Operator::create<Operator::LSHIFT>();
template<>
Operator& Operator::create<Operator::RSHIFT>();
template<>
Operator& Operator::create<Operator::LT>();
template<>
Operator& Operator::create<Operator::LE>();
template<>
Operator& Operator::create<Operator::GT>();
template<>
Operator& Operator::create<Operator::GE>();
template<>
Operator& Operator::create<Operator::EQ>();
template<>
Operator& Operator::create<Operator::NE>();
template<>
Operator& Operator::create<Operator::BITWISE_AND>();
template<>
Operator& Operator::create<Operator::BITWISE_XOR>();
template<>
Operator& Operator::create<Operator::BITWISE_OR>();
template<>
Operator& Operator::create<Operator::LOGICAL_AND>();
template<>
Operator& Operator::create<Operator::LOGICAL_OR>();
template<>
Operator& Operator::create<Operator::ASSIGN>();
template<>
Operator& Operator::create<Operator::ADD_ASSIGN>();
template<>
Operator& Operator::create<Operator::SUB_ASSIGN>();
template<>
Operator& Operator::create<Operator::MUL_ASSIGN>();
template<>
Operator& Operator::create<Operator::DIV_ASSIGN>();
template<>
Operator& Operator::create<Operator::AND_ASSIGN>();
template<>
Operator& Operator::create<Operator::OR_ASSIGN>();
template<>
Operator& Operator::create<Operator::LS_ASSIGN>();
template<>
Operator& Operator::create<Operator::RS_ASSIGN>();

/* Ternary operator */
template<>
Operator& Operator::create<Operator::TERNARY_IF>();

} // namespace of mcld

#endif

