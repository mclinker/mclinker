//===- GroupCmd.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/GroupCmd.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/MC/Attribute.h>
#include <mcld/Support/raw_ostream.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/InputTree.h>
#include <mcld/LD/GroupReader.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GroupCmd
//===----------------------------------------------------------------------===//
GroupCmd::GroupCmd(InputTree& pInputTree,
                   InputBuilder& pBuilder,
                   GroupReader& pGroupReader,
                   const LinkerConfig& pConfig)
  : ScriptCommand(ScriptCommand::Group),
    m_InputTree(pInputTree),
    m_Builder(pBuilder),
    m_GroupReader(pGroupReader),
    m_Config(pConfig)
{
}

GroupCmd::~GroupCmd()
{
}

void GroupCmd::dump() const
{
  mcld::outs() << "GROUP ( ";
  bool prev = false, cur = false;
  for (ScriptInput::const_iterator it = scriptInput().begin(),
    ie = scriptInput().end(); it != ie; ++it) {
    cur = (*it).asNeeded();
    if (!prev && cur)
      mcld::outs() << "AS_NEEDED ( ";
    if (prev && !cur)
      mcld::outs() << " )";
    mcld::outs() << (*it).path().native() << " ";
    prev = cur;
  }
  if (!scriptInput().empty() && scriptInput().back().asNeeded())
    mcld::outs() << " )";
  mcld::outs() << " )\n";
}

void GroupCmd::activate()
{
  // construct the Group tree
  m_Builder.setCurrentTree(m_InputTree);
  // --start-group
  m_Builder.enterGroup();
  InputTree::iterator group = m_Builder.getCurrentNode();

  for (ScriptInput::const_iterator it = scriptInput().begin(),
    ie = scriptInput().end(); it != ie; ++it) {
    if ((*it).asNeeded())
      m_Builder.getAttributes().setAsNeeded();
    else
      m_Builder.getAttributes().unsetAsNeeded();

    m_Builder.createNode<InputTree::Positional>(
      (*it).path().filename().native(), (*it).path(), Input::Unknown);

    Input* input = *m_Builder.getCurrentNode();
    assert(input != NULL);
    if (!m_Builder.setMemory(*input, FileHandle::ReadOnly))
      error(diag::err_cannot_open_input) << input->name() << input->path();
    m_Builder.setContext(*input);
  }

  // --end-group
  m_Builder.exitGroup();

  // read the group
  m_GroupReader.readGroup(group, m_InputTree.end(), m_Builder, m_Config);

}

