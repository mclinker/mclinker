//===- EntryCmd.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ENTRY_COMMAND_INTERFACE_H
#define MCLD_ENTRY_COMMAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>
#include <string>

namespace mcld
{

class LinkerScript;
class StringRef;

/** \class EntryCmd
 *  \brief This class defines the interfaces to Entry command.
 */

class EntryCmd : public ScriptCommand
{
public:
  EntryCmd(const std::string& pEntry, LinkerScript& pScript);
  ~EntryCmd();

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::Entry;
  }

  void activate();

private:
  std::string m_Entry;
  LinkerScript& m_Script;
};

} // namespace of mcld

#endif

