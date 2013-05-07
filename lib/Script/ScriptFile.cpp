//===- ScriptFile.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/ScriptFile.h>
#include <mcld/Script/ScriptInput.h>
#include <mcld/Script/ScriptCommand.h>
#include <mcld/Script/EntryCmd.h>
#include <mcld/Script/OutputFormatCmd.h>
#include <mcld/Script/GroupCmd.h>
#include <mcld/Script/SearchDirCmd.h>
#include <mcld/Script/OutputArchCmd.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/Operand.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/InputBuilder.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/InputTree.h>
#include <mcld/ADT/HashEntry.h>
#include <mcld/ADT/HashTable.h>
#include <mcld/ADT/StringHash.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/ManagedStatic.h>
#include <cassert>

using namespace mcld;

typedef HashEntry<std::string,
                  void*,
                  hash::StringCompare<std::string> > ParserStrEntry;
typedef HashTable<ParserStrEntry,
                  hash::StringHash<hash::ELF>,
                  EntryFactory<ParserStrEntry> > ParserStrPool;
static llvm::ManagedStatic<ParserStrPool> g_ParserStrPool;

//===----------------------------------------------------------------------===//
// ScriptFile
//===----------------------------------------------------------------------===//
ScriptFile::ScriptFile(Kind pKind, Input& pInput, InputBuilder& pBuilder)
  : m_Type(InputData),
    m_Kind(pKind),
    m_Name(pInput.name()),
    m_pInputTree(NULL),
    m_Builder(pBuilder)
{
  m_Data.input = &pInput;
  if (pInput.hasMemArea() && pInput.memArea()->hasHandler())
    m_Name.assign(pInput.path().native());
  // FIXME: move creation of input tree out of ScriptFile.
  m_pInputTree = new InputTree();
}

ScriptFile::ScriptFile(Kind pKind,
                       const std::string& pName,
                       std::string& pData,
                       InputBuilder& pBuilder)
  : m_Type(StringData),
    m_Kind(pKind),
    m_Name(pName),
    m_pInputTree(NULL),
    m_Builder(pBuilder)
{
  m_Data.str = &pData;
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

void ScriptFile::addEntryPoint(const std::string& pSymbol, LinkerScript& pScript)
{
  m_CommandQueue.push_back(new EntryCmd(pSymbol, pScript));
}

void ScriptFile::addOutputFormatCmd(const std::string& pName)
{
  m_CommandQueue.push_back(new OutputFormatCmd(pName));
}

void ScriptFile::addOutputFormatCmd(const std::string& pDefault,
                                    const std::string& pBig,
                                    const std::string& pLittle)
{
  m_CommandQueue.push_back(new OutputFormatCmd(pDefault, pBig, pLittle));
}

void ScriptFile::addScriptInput(const std::string& pPath)
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

void ScriptFile::addSearchDirCmd(const std::string& pPath,
                                 LinkerScript& pScript)
{
  m_CommandQueue.push_back(new SearchDirCmd(pPath, pScript));
}

void ScriptFile::addOutputArchCmd(const std::string& pArch)
{
  m_CommandQueue.push_back(new OutputArchCmd(pArch));
}

void ScriptFile::addAssignment(LinkerScript& pLDScript,
                               const std::string& pSymbolName,
                               Assignment::Type pType)
{
  m_CommandQueue.push_back(
    new Assignment(pLDScript, pType,
                   *(Operand::create(Operand::SYMBOL, pSymbolName)),
                   *(RpnExpr::create())));
}

void ScriptFile::addExprToken(ExprToken* pToken)
{
  ScriptCommand* cmd = back();
  switch (cmd->getKind()) {
  case ScriptCommand::Assignment: {
    Assignment* assignment = llvm::cast<Assignment>(cmd);
    assignment->getRpnExpr().append(pToken);
    break;
  }
  default:
    assert(0 && "Invalid command to add expression token.\n");
    break;
  }
}

const std::string& ScriptFile::createParserStr(const char* pText,
                                               size_t pLength)
{
  bool exist = false;
  ParserStrEntry* entry =
    g_ParserStrPool->insert(std::string(pText, pLength), exist);
  return entry->key();
}

void ScriptFile::clearParserStrPool()
{
  g_ParserStrPool->clear();
}

