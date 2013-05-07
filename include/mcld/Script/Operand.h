//===- Operand.h ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OPERAND_INTERFACE_H
#define MCLD_OPERAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ExprToken.h>
#include <mcld/Support/Allocators.h>
#include <mcld/Config/Config.h>
#include <llvm/Support/DataTypes.h>
#include <string>

namespace mcld
{

/** \class Operand
 *  \brief This class defines the interfaces to an operand token.
 */

class Operand : public ExprToken
{
public:
  enum Type {
    SYMBOL,
    SECTION,
    INTEGER,
    DOT,
    UNKNOWN
  };

private:
  friend class Chunk<Operand, MCLD_SYMBOLS_PER_INPUT>;
  Operand();
  Operand(uint64_t pValue);
  Operand(Type pType, const std::string& pValue);

public:
  ~Operand();

  Type type() const { return m_Type; }

  void dump() const;

  const std::string& strVal() const { return *(m_Data.strVal); }

  uint64_t intVal() const { return m_Data.intVal; }

  static bool classof(const ExprToken* pToken)
  {
    return pToken->kind() == ExprToken::Opd;
  }

  /* factory method */
  static Operand* create(uint64_t pValue);
  static Operand* create(Type pType, const std::string& pValue);
  static void destroy(Operand*& pOperand);
  static void clear();

private:
  union Data
  {
    const std::string* strVal;
    uint64_t intVal;
  };

private:
  Type m_Type;
  Data m_Data;
};

} // namespace of mcld

#endif

