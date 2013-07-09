//===- AssertCmd.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ASSERT_COMMAND_INTERFACE_H
#define MCLD_ASSERT_COMMAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>
#include <string>

namespace mcld
{

class RpnExpr;

/** \class AssertCmd
 *  \brief This class defines the interfaces to assert command.
 */

class AssertCmd : public ScriptCommand
{
public:
  AssertCmd(RpnExpr& pRpnExpr, const std::string& pMessage);

  ~AssertCmd();

  const RpnExpr& getRpnExpr() const { return m_RpnExpr; }
  RpnExpr&       getRpnExpr()       { return m_RpnExpr; }

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::ASSERT;
  }

  void activate();

private:
  RpnExpr& m_RpnExpr;
  std::string m_Message;
};

} // namespace of mcld

#endif
