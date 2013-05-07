//===- Assignment.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ASSIGNMENT_COMMAND_INTERFACE_H
#define MCLD_ASSIGNMENT_COMMAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>

namespace mcld
{

class LinkerScript;
class RpnExpr;
class Operand;

/** \class Assignment
 *  \brief This class defines the interfaces to assignment command.
 */

class Assignment : public ScriptCommand
{
public:
  enum Type {
    DEFAULT,
    HIDDEN,
    PROVIDE,
    PROVIDE_HIDDEN
  };

public:
  Assignment(LinkerScript& pScript,
             Type pType,
             Operand& pSymbol,
             RpnExpr& pRpnExpr);

  ~Assignment();

  Assignment& operator=(const Assignment& pAssignment);

  Type type() const { return m_Type; }

  const Operand& symbol() const { return m_Symbol; }

  const RpnExpr& getRpnExpr() const { return m_RpnExpr; }
  RpnExpr&       getRpnExpr()       { return m_RpnExpr; }

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::Assignment;
  }

  void activate();

private:
  LinkerScript& m_Script;
  Type m_Type;
  Operand& m_Symbol;
  RpnExpr& m_RpnExpr;
};

} // namespace of mcld

#endif

