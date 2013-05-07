//===- SearchDirCmd.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SEARCHDIR_COMMAND_INTERFACE_H
#define MCLD_SEARCHDIR_COMMAND_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>
#include <string>

namespace mcld
{

class LinkerScript;

/** \class SearchDirCmd
 *  \brief This class defines the interfaces to SEARCH_DIR command.
 */

class SearchDirCmd : public ScriptCommand
{
public:
  SearchDirCmd(const std::string& pPath, LinkerScript& pScript);
  ~SearchDirCmd();

  void dump() const;

  void activate();

private:
  std::string m_Path;
  LinkerScript& m_Script;
};

} // namespace of mcld

#endif

