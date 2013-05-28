//===- EntryCmd.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/EntryCmd.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/LinkerScript.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// EntryCmd
//===----------------------------------------------------------------------===//
EntryCmd::EntryCmd(const std::string& pEntry, LinkerScript& pScript)
  : ScriptCommand(ScriptCommand::Entry),
    m_Entry(pEntry),
    m_Script(pScript)
{
}

EntryCmd::~EntryCmd()
{
}

void EntryCmd::dump() const
{
  mcld::outs() << "ENTRY ( " << m_Entry << " )\n";
}

void EntryCmd::activate()
{
  if (!m_Script.hasEntry())
    m_Script.setEntry(m_Entry);
}

