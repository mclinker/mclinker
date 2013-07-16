//===- Operator.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_OPERATOR_INTERFACE_H
#define MCLD_SCRIPT_OPERATOR_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ExprToken.h>
#include <llvm/Support/DataTypes.h>

namespace mcld
{

class Operand;
class IntOperand;
class Module;
class TargetLDBackend;

/** \class Operator
 *  \brief This class defines the interfaces to an operator token.
 */

class Operator : public ExprToken
{
public:
  enum Arity {
    Nullary,
    Unary,
    Binary,
    Ternary
  };

  enum Type {
    /* arithmetic operator */
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
    RS_ASSIGN   = 31,
    /* function */
    ABSOLUTE               = 32,
    ADDR                   = 33,
    ALIGN                  = 34,
    ALIGNOF                = 35,
    BLOCK                  = 36,
    DATA_SEGMENT_ALIGN     = 37,
    DATA_SEGMENT_END       = 38,
    DATA_SEGMENT_RELRO_END = 39,
    DEFINED                = 40,
    LENGTH                 = 41,
    LOADADDR               = 42,
    MAX                    = 43,
    MIN                    = 44,
    NEXT                   = 45,
    ORIGIN                 = 46,
    SEGMENT_START          = 47,
    SIZEOF                 = 48,
    SIZEOF_HEADERS         = 49,
    MAXPAGESIZE            = 50,
    COMMONPAGESIZE         = 51
  };

  static const char* OpNames[];

protected:
  Operator(const Module& pModule,
           const TargetLDBackend& pBackend,
           Arity pArity,
           Type pType);

  const IntOperand* result() const { return m_pIntOperand; }
  IntOperand*       result()       { return m_pIntOperand; }

  const Module& module() const { return m_Module; }

  const TargetLDBackend& backend() const { return m_LDBackend; }

public:
  virtual ~Operator();

  Arity arity() const { return m_Arity; }

  Type type() const { return m_Type; }

  virtual void dump() const;

  virtual IntOperand* eval() = 0;

  virtual void appendOperand(Operand* pOperand) = 0;

  static bool classof(const ExprToken* pToken)
  {
    return pToken->kind() == ExprToken::OPERATOR;
  }

  template<Operator::Type TYPE>
  static Operator& create(const Module& pModule,
                          const TargetLDBackend& pBackend);

private:
  const Module& m_Module;
  const TargetLDBackend& m_LDBackend;
  Arity m_Arity;
  Type m_Type;
  IntOperand* m_pIntOperand;
};

/* Nullary operator */
template<>
Operator& Operator::create<Operator::SIZEOF_HEADERS>(const Module&,
                                                     const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::MAXPAGESIZE>(const Module&,
                                                  const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::COMMONPAGESIZE>(const Module&,
                                                     const TargetLDBackend&);

/* Unary operator */
template<>
Operator& Operator::create<Operator::UNARY_PLUS>(const Module&,
                                                 const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::UNARY_MINUS>(const Module&,
                                                  const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LOGICAL_NOT>(const Module&,
                                                  const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::BITWISE_NOT>(const Module&,
                                                  const TargetLDBackend&);

template<>
Operator& Operator::create<Operator::ABSOLUTE>(const Module&,
                                               const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::ADDR>(const Module&,
                                           const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::ALIGNOF>(const Module&,
                                              const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::DATA_SEGMENT_END>(const Module&,
                                                       const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::DEFINED>(const Module&,
                                              const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LENGTH>(const Module&,
                                             const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LOADADDR>(const Module&,
                                               const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::NEXT>(const Module&,
                                           const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::ORIGIN>(const Module&,
                                             const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::SIZEOF>(const Module&,
                                             const TargetLDBackend&);

/* Binary operator */
template<>
Operator& Operator::create<Operator::MUL>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::DIV>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::MOD>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::ADD>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::SUB>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LSHIFT>(const Module&,
                                             const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::RSHIFT>(const Module&,
                                             const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LT>(const Module&,
                                         const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LE>(const Module&,
                                         const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::GT>(const Module&,
                                         const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::GE>(const Module&,
                                         const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::EQ>(const Module&,
                                         const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::NE>(const Module&,
                                         const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::BITWISE_AND>(const Module&,
                                                  const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::BITWISE_XOR>(const Module&,
                                                  const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::BITWISE_OR>(const Module&,
                                                 const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LOGICAL_AND>(const Module&,
                                                  const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::LOGICAL_OR>(const Module&,
                                                 const TargetLDBackend&);

template<>
Operator& Operator::create<Operator::ALIGN>(const Module&,
                                            const TargetLDBackend&);
template<>
Operator&
Operator::create<Operator::DATA_SEGMENT_ALIGN>(const Module&,
                                               const TargetLDBackend&);
template<>
Operator&
Operator::create<Operator::DATA_SEGMENT_RELRO_END>(const Module&,
                                                   const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::MAX>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::MIN>(const Module&,
                                          const TargetLDBackend&);
template<>
Operator& Operator::create<Operator::SEGMENT_START>(const Module&,
                                                    const TargetLDBackend&);

/* Ternary operator */
template<>
Operator& Operator::create<Operator::TERNARY_IF>(const Module&,
                                                 const TargetLDBackend&);

} // namespace of mcld

#endif

