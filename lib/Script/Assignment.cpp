//===- Assignment.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/Assignment.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/Operand.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LinkerScript.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Assignment
//===----------------------------------------------------------------------===//
Assignment::Assignment(LinkerScript& pScript,
                       Type pType,
                       Operand& pSymbol,
                       RpnExpr& pRpnExpr)
  : ScriptCommand(ScriptCommand::Assignment),
    m_Script(pScript),
    m_Type(pType),
    m_Symbol(pSymbol),
    m_RpnExpr(pRpnExpr)
{
}

Assignment::~Assignment()
{
}

Assignment& Assignment::operator=(const Assignment& pAssignment)
{
  return *this;
}

void Assignment::dump() const
{
  switch (type()) {
  case DEFAULT:
    break;
  case HIDDEN:
    mcld::outs() << "HIDDEN ( ";
    break;
  case PROVIDE:
    mcld::outs() << "PROVIDE ( ";
    break;
  case PROVIDE_HIDDEN:
    mcld::outs() << "PROVIDE_HIDDEN ( ";
    break;
  default:
    break;
  }

  m_Symbol.dump();

  mcld::outs() << "= ";

  m_RpnExpr.dump();

  if (type() != DEFAULT)
    mcld::outs() << " )";

  mcld::outs() << "\n";
}

void Assignment::activate()
{
  switch (m_Symbol.type()) {
  case Operand::SYMBOL: {
    LDSymbol* sym = NULL;
    m_Script.assignments().push_back(std::make_pair(sym, *this));
    break;
  }
  case Operand::DOT:
    // TODO
    break;
  default:
    assert(0 && "Vaild lvalue required as left operand of assignment!");
    break;
  }
}

