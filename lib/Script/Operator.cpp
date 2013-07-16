//===- Operator.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/Operator.h>
#include <mcld/Script/NullaryOp.h>
#include <mcld/Script/UnaryOp.h>
#include <mcld/Script/BinaryOp.h>
#include <mcld/Script/TernaryOp.h>
#include <mcld/Script/Operand.h>
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
  ">>=",
  "ABSOLUTE",
  "ADDR",
  "ALIGN",
  "ALIGNOF",
  "BLOCK",
  "DATA_SEGMENT_ALIGN",
  "DATA_SEGMENT_END",
  "DATA_SEGMENT_RELRO_END",
  "DEFINED",
  "LENGTH",
  "LOADADDR",
  "MAX",
  "MIN",
  "NEXT",
  "ORIGIN",
  "SEGMENT_START",
  "SIZEOF",
  "SIZEOF_HEADERS",
  "MAXPAGESIZE",
  "COMMONPAGESIZE"
};

Operator::Operator(const Module& pModule,
                   const TargetLDBackend& pBackend,
                   Arity pArity,
                   Type pType)
  : ExprToken(ExprToken::OPERATOR),
    m_Module(pModule),
    m_LDBackend(pBackend),
    m_Arity(pArity),
    m_Type(pType)
{
  m_pIntOperand = IntOperand::create(0);
}

Operator::~Operator()
{
}

void Operator::dump() const
{
  mcld::outs() << OpNames[type()];
}

/* Nullary operator */
template<>
Operator&
Operator::create<Operator::SIZEOF_HEADERS>(const Module& pModule,
                                           const TargetLDBackend& pBackend)
{
  static NullaryOp<Operator::SIZEOF_HEADERS> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::MAXPAGESIZE>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static NullaryOp<Operator::MAXPAGESIZE> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::COMMONPAGESIZE>(const Module& pModule,
                                           const TargetLDBackend& pBackend)
{
  static NullaryOp<Operator::COMMONPAGESIZE> op(pModule, pBackend);
  return op;
}

/* Unary operator */
template<>
Operator&
Operator::create<Operator::UNARY_PLUS>(const Module& pModule,
                                       const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::UNARY_PLUS> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::UNARY_MINUS>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::UNARY_MINUS> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::LOGICAL_NOT>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::LOGICAL_NOT> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::BITWISE_NOT>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::BITWISE_NOT> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::ABSOLUTE>(const Module& pModule,
                                               const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::ABSOLUTE> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::ADDR>(const Module& pModule,
                                           const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::ADDR> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::ALIGNOF>(const Module& pModule,
                                              const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::ALIGNOF> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::DATA_SEGMENT_END>(const Module& pModule,
                                             const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::DATA_SEGMENT_END> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::DEFINED>(const Module& pModule,
                                              const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::DEFINED> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::LENGTH>(const Module& pModule,
                                             const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::LENGTH> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::LOADADDR>(const Module& pModule,
                                               const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::LOADADDR> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::NEXT>(const Module& pModule,
                                           const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::NEXT> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::ORIGIN>(const Module& pModule,
                                             const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::ORIGIN> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::SIZEOF>(const Module& pModule,
                                             const TargetLDBackend& pBackend)
{
  static UnaryOp<Operator::SIZEOF> op(pModule, pBackend);
  return op;
}

/* Binary operator */
template<>
Operator& Operator::create<Operator::MUL>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::MUL> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::DIV>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::DIV> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::MOD>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::MOD> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::ADD>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::ADD> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::SUB>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::SUB> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::LSHIFT>(const Module& pModule,
                                             const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::LSHIFT> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::RSHIFT>(const Module& pModule,
                                             const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::RSHIFT> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::LT>(const Module& pModule,
                                         const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::LT> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::LE>(const Module& pModule,
                                         const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::LE> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::GT>(const Module& pModule,
                                         const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::GT> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::GE>(const Module& pModule,
                                         const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::GE> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::EQ>(const Module& pModule,
                                         const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::EQ> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::NE>(const Module& pModule,
                                         const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::NE> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::BITWISE_AND>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::BITWISE_AND> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::BITWISE_XOR>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::BITWISE_XOR> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::BITWISE_OR>(const Module& pModule,
                                       const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::BITWISE_OR> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::LOGICAL_AND>(const Module& pModule,
                                        const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::LOGICAL_AND> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::LOGICAL_OR>(const Module& pModule,
                                       const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::LOGICAL_OR> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::ALIGN>(const Module& pModule,
                                            const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::ALIGN> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::DATA_SEGMENT_RELRO_END>(
  const Module& pModule, const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::DATA_SEGMENT_RELRO_END> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::MAX>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::MAX> op(pModule, pBackend);
  return op;
}

template<>
Operator& Operator::create<Operator::MIN>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::MIN> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::SEGMENT_START>(const Module& pModule,
                                          const TargetLDBackend& pBackend)
{
  static BinaryOp<Operator::SEGMENT_START> op(pModule, pBackend);
  return op;
}

/* Ternary operator */
template<>
Operator&
Operator::create<Operator::TERNARY_IF>(const Module& pModule,
                                       const TargetLDBackend& pBackend)
{
  static TernaryOp<Operator::TERNARY_IF> op(pModule, pBackend);
  return op;
}

template<>
Operator&
Operator::create<Operator::DATA_SEGMENT_ALIGN>(const Module& pModule,
                                               const TargetLDBackend& pBackend)
{
  static TernaryOp<Operator::DATA_SEGMENT_ALIGN> op(pModule, pBackend);
  return op;
}
