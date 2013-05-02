//===- Assignment.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LinkerScript/Assignment.h>
#include <mcld/LD/LinkerScript/RpnExpr.h>
#include <mcld/LD/LinkerScript/Operand.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LinkerScript.h>

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
  // TODO
}

