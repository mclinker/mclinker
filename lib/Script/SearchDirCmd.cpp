//===- SearchDirCmd.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/SearchDirCmd.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LinkerScript.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// SearchDirCmd
//===----------------------------------------------------------------------===//
SearchDirCmd::SearchDirCmd(const std::string& pPath, LinkerScript& pScript)
  : ScriptCommand(ScriptCommand::SearchDir),
    m_Path(pPath),
    m_Script(pScript)
{
}

SearchDirCmd::~SearchDirCmd()
{
}

void SearchDirCmd::dump() const
{
  mcld::outs() << "SEARCH_DIR ( " << m_Path << " )\n";
}

void SearchDirCmd::activate()
{
  m_Script.directories().insert(m_Path);
}

