//===- ScriptFile.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LinkerScript/ScriptFile.h>
#include <mcld/LD/LinkerScript/ScriptInput.h>
#include <mcld/LD/LinkerScript/ScriptCommand.h>
#include <mcld/LD/LinkerScript/OutputFormatCmd.h>
#include <mcld/LD/LinkerScript/GroupCmd.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/InputTree.h>
#include <llvm/Support/Casting.h>
#include <cassert>

using namespace mcld;

ScriptFile::ScriptFile(Input& pInput, InputBuilder& pBuilder)
  : m_Name(pInput.path().native()),
    m_Script(pInput),
    m_pInputTree(NULL),
    m_Builder(pBuilder)
{
  // FIXME: move creation of input tree out of Archive.
  m_pInputTree = new InputTree();
}

ScriptFile::~ScriptFile()
{
  for (iterator it = begin(), ie = end(); it != ie; ++it)
    delete *it;
  if (NULL != m_pInputTree)
    delete m_pInputTree;
}

void ScriptFile::dump() const
{
  for (const_iterator it = begin(), ie = end(); it != ie; ++it)
    (*it)->dump();
}

void ScriptFile::activate()
{
  for (const_iterator it = begin(), ie = end(); it != ie; ++it)
    (*it)->activate();
}

void ScriptFile::addOutputFormatCmd(const char* pName)
{
  m_CommandQueue.push_back(new OutputFormatCmd(pName));
}

void ScriptFile::addOutputFormatCmd(const char* pDefault,
                                    const char* pBig,
                                    const char* pLittle)
{
  m_CommandQueue.push_back(new OutputFormatCmd(pDefault, pBig, pLittle));
}

void ScriptFile::addScriptInput(const char* pPath)
{
  assert(!m_CommandQueue.empty());
  ScriptCommand* cmd = back();
  switch (cmd->getKind()) {
  case ScriptCommand::Group:
    llvm::cast<GroupCmd>(cmd)->scriptInput().append(pPath);
    break;
  default:
    assert(0 && "Invalid command to add script input!");
    break;
  }
}

void ScriptFile::setAsNeeded(bool pEnable)
{
  assert(!m_CommandQueue.empty());
  ScriptCommand* cmd = back();
  switch (cmd->getKind()) {
  case ScriptCommand::Group:
    llvm::cast<GroupCmd>(cmd)->scriptInput().setAsNeeded(pEnable);
    break;
  default:
    assert(0 && "Invalid command to use AS_NEEDED!");
    break;
  }
}

void ScriptFile::addGroupCmd(GroupReader& pGroupReader,
                             const LinkerConfig& pConfig)
{
  m_CommandQueue.push_back(
    new GroupCmd(*m_pInputTree, m_Builder, pGroupReader, pConfig));
}

