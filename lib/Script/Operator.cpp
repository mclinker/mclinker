//===- Operator.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/Operator.h>
#include <mcld/Script/UnaryOp.h>
#include <mcld/Script/BinaryOp.h>
#include <mcld/Script/TernaryOp.h>
#include <mcld/Support/raw_ostream.h>

using namespace mcld;
//===----------------------------------------------------------------------===//
// Operator
//===----------------------------------------------------------------------===//
const char* Operator::OpNames[] = {
  "+",
  "-",
  "!",
  "~",
  "*",
  "/",
  "%",
  "+",
  "-",
  "<<",
  ">>",
  "<",
  "<=",
  ">",
  ">=",
  "==",
  "!=",
  "&",
  "^",
  "|",
  "&&",
  "||",
  "?:",
  "=",
  "+=",
  "-=",
  "*=",
  "/=",
  "&=",
  "|=",
  "<<=",
  ">>="
};

void Operator::dump() const
{
  mcld::outs() << OpNames[type()] << " ";
}

template<>
Operator& Operator::create<Operator::UNARY_PLUS>()
{
  static UnaryOp<Operator::UNARY_PLUS> op;
  return op;
}

template<>
Operator& Operator::create<Operator::UNARY_MINUS>()
{
  static UnaryOp<Operator::UNARY_MINUS> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LOGICAL_NOT>()
{
  static UnaryOp<Operator::LOGICAL_NOT> op;
  return op;
}

template<>
Operator& Operator::create<Operator::BITWISE_NOT>()
{
  static UnaryOp<Operator::BITWISE_NOT> op;
  return op;
}

template<>
Operator& Operator::create<Operator::MUL>()
{
  static BinaryOp<Operator::MUL> op;
  return op;
}

template<>
Operator& Operator::create<Operator::DIV>()
{
  static BinaryOp<Operator::DIV> op;
  return op;
}

template<>
Operator& Operator::create<Operator::MOD>()
{
  static BinaryOp<Operator::MOD> op;
  return op;
}

template<>
Operator& Operator::create<Operator::ADD>()
{
  static BinaryOp<Operator::ADD> op;
  return op;
}

template<>
Operator& Operator::create<Operator::SUB>()
{
  static BinaryOp<Operator::SUB> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LSHIFT>()
{
  static BinaryOp<Operator::LSHIFT> op;
  return op;
}

template<>
Operator& Operator::create<Operator::RSHIFT>()
{
  static BinaryOp<Operator::RSHIFT> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LT>()
{
  static BinaryOp<Operator::LT> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LE>()
{
  static BinaryOp<Operator::LE> op;
  return op;
}

template<>
Operator& Operator::create<Operator::GT>()
{
  static BinaryOp<Operator::GT> op;
  return op;
}

template<>
Operator& Operator::create<Operator::GE>()
{
  static BinaryOp<Operator::GE> op;
  return op;
}

template<>
Operator& Operator::create<Operator::EQ>()
{
  static BinaryOp<Operator::EQ> op;
  return op;
}

template<>
Operator& Operator::create<Operator::NE>()
{
  static BinaryOp<Operator::NE> op;
  return op;
}

template<>
Operator& Operator::create<Operator::BITWISE_AND>()
{
  static BinaryOp<Operator::BITWISE_AND> op;
  return op;
}

template<>
Operator& Operator::create<Operator::BITWISE_XOR>()
{
  static BinaryOp<Operator::BITWISE_XOR> op;
  return op;
}

template<>
Operator& Operator::create<Operator::BITWISE_OR>()
{
  static BinaryOp<Operator::BITWISE_OR> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LOGICAL_AND>()
{
  static BinaryOp<Operator::LOGICAL_AND> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LOGICAL_OR>()
{
  static BinaryOp<Operator::LOGICAL_OR> op;
  return op;
}

template<>
Operator& Operator::create<Operator::ASSIGN>()
{
  static BinaryOp<Operator::ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::ADD_ASSIGN>()
{
  static BinaryOp<Operator::ADD_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::SUB_ASSIGN>()
{
  static BinaryOp<Operator::SUB_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::MUL_ASSIGN>()
{
  static BinaryOp<Operator::MUL_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::DIV_ASSIGN>()
{
  static BinaryOp<Operator::DIV_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::AND_ASSIGN>()
{
  static BinaryOp<Operator::AND_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::OR_ASSIGN>()
{
  static BinaryOp<Operator::OR_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::LS_ASSIGN>()
{
  static BinaryOp<Operator::LS_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::RS_ASSIGN>()
{
  static BinaryOp<Operator::RS_ASSIGN> op;
  return op;
}

template<>
Operator& Operator::create<Operator::TERNARY_IF>()
{
  static TernaryOp<Operator::TERNARY_IF> op;
  return op;
}

