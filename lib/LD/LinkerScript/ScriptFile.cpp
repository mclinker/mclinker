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
#include <mcld/LD/LinkerScript/EntryCmd.h>
#include <mcld/LD/LinkerScript/OutputFormatCmd.h>
#include <mcld/LD/LinkerScript/GroupCmd.h>
#include <mcld/LD/LinkerScript/SearchDirCmd.h>
#include <mcld/LD/LinkerScript/OutputArchCmd.h>
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

void ScriptFile::addEntryPoint(const StrToken& pSymbol, LinkerScript& pScript)
{
  std::string sym(pSymbol.text, pSymbol.length);
  m_CommandQueue.push_back(new EntryCmd(sym, pScript));
}

void ScriptFile::addOutputFormatCmd(const StrToken& pName)
{
  std::string format(pName.text, pName.length);
  m_CommandQueue.push_back(new OutputFormatCmd(format));
}

void ScriptFile::addOutputFormatCmd(const StrToken& pDefault,
                                    const StrToken& pBig,
                                    const StrToken& pLittle)
{
  std::string def(pDefault.text, pDefault.length);
  std::string big(pBig.text, pDefault.length);
  std::string lit(pLittle.text, pLittle.length);
  m_CommandQueue.push_back(new OutputFormatCmd(def, big, lit));
}

void ScriptFile::addScriptInput(const StrToken& pPath)
{
  assert(!m_CommandQueue.empty());
  std::string path(pPath.text, pPath.length);
  ScriptCommand* cmd = back();
  switch (cmd->getKind()) {
  case ScriptCommand::Group:
    llvm::cast<GroupCmd>(cmd)->scriptInput().append(path);
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

void ScriptFile::addSearchDirCmd(const StrToken& pPath,
                                 LinkerScript& pScript)
{
  std::string path(pPath.text, pPath.length);
  m_CommandQueue.push_back(new SearchDirCmd(path, pScript));
}

void ScriptFile::addOutputArchCmd(const StrToken& pArch)
{
  std::string arch(pArch.text, pArch.length);
  m_CommandQueue.push_back(new OutputArchCmd(arch));
}
