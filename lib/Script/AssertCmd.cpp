//===- AssertCmd.cpp ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/AssertCmd.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Support/raw_ostream.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// AssertCmd
//===----------------------------------------------------------------------===//
AssertCmd::AssertCmd(RpnExpr& pRpnExpr, const std::string& pMessage)
  : ScriptCommand(ScriptCommand::Assert),
    m_RpnExpr(pRpnExpr),
    m_Message(pMessage)
{
}

AssertCmd::~AssertCmd()
{
}

void AssertCmd::dump() const
{
  mcld::outs() << "Assert ( ";

  m_RpnExpr.dump();

  mcld::outs() << " , " << m_Message << " )\n";
}

void AssertCmd::activate()
{
 // TODO
}
