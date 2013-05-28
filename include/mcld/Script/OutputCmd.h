//===- OutputCmd.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_OUTPUT_COMMAND_INTERFACE_H
#define MCLD_OUTPUT_COMMAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>
#include <string>

namespace mcld
{

class LinkerScript;

/** \class OutputCmd
 *  \brief This class defines the interfaces to Output command.
 */

class OutputCmd : public ScriptCommand
{
public:
  OutputCmd(const std::string& pOutputFile, LinkerScript& pScript);

  ~OutputCmd();

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::Output;
  }

  void activate();

private:
  std::string m_OutputFile;
  LinkerScript& m_Script;
};

} // namespace of mcld

#endif

