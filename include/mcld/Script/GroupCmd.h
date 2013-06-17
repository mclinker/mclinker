//===- GroupCmd.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GROUP_COMMAND_INTERFACE_H
#define MCLD_GROUP_COMMAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>

namespace mcld
{

class ScriptInput;
class InputTree;
class InputBuilder;
class GroupReader;
class LinkerConfig;
class LinkerScript;

/** \class GroupCmd
 *  \brief This class defines the interfaces to Group command.
 */

class GroupCmd : public ScriptCommand
{
public:
  GroupCmd(ScriptInput& pScriptInput,
           InputTree& pInputTree,
           InputBuilder& pBuilder,
           GroupReader& m_GroupReader,
           const LinkerConfig& pConfig,
           const LinkerScript& pScript);
  ~GroupCmd();

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::Group;
  }

  void activate();

private:
  ScriptInput& m_ScriptInput;
  InputTree& m_InputTree;
  InputBuilder& m_Builder;
  GroupReader& m_GroupReader;
  const LinkerConfig& m_Config;
  const LinkerScript& m_Script;
};

} // namespace of mcld

#endif

