//===- ScriptFile.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/ScriptFile.h>
#include <mcld/Script/StringList.h>
#include <mcld/Script/ScriptCommand.h>
#include <mcld/Script/EntryCmd.h>
#include <mcld/Script/OutputFormatCmd.h>
#include <mcld/Script/GroupCmd.h>
#include <mcld/Script/OutputCmd.h>
#include <mcld/Script/SearchDirCmd.h>
#include <mcld/Script/OutputArchCmd.h>
#include <mcld/Script/AssertCmd.h>
#include <mcld/Script/SectionsCmd.h>
#include <mcld/Script/RpnExpr.h>
#include <mcld/Script/Operand.h>
#include <mcld/Script/StrToken.h>
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
  : m_Kind(pKind),
    m_Input(pInput),
    m_Name(pInput.path().native()),
    m_pInputTree(NULL),
    m_Builder(pBuilder),
    m_HasSectionsCmd(false),
    m_InSectionsCmd(false),
    m_InOutputSectDesc(false)
{
  // FIXME: move creation of input tree out of ScriptFile.
  m_pInputTree = new InputTree();
}

ScriptFile::~ScriptFile()
{
  for (iterator it = begin(), ie = end(); it != ie; ++it) {
    if (*it != NULL)
      delete *it;
  }
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
  EntryCmd* entry = new EntryCmd(pSymbol, pScript);

  if (m_InSectionsCmd) {
    assert(!m_CommandQueue.empty());
    SectionsCmd* sections = llvm::cast<SectionsCmd>(back());
    sections->push_back(entry);
  } else {
    m_CommandQueue.push_back(entry);
  }
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

void ScriptFile::addGroupCmd(StringList& pStringList,
                             GroupReader& pGroupReader,
                             const LinkerConfig& pConfig,
                             const LinkerScript& pScript)
{
  m_CommandQueue.push_back(
    new GroupCmd(pStringList, *m_pInputTree, m_Builder, pGroupReader, pConfig,
                 pScript));
}

void ScriptFile::addOutputCmd(const std::string& pFileName,
                              LinkerScript& pScript)
{
  m_CommandQueue.push_back(new OutputCmd(pFileName, pScript));
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

void ScriptFile::addAssertCmd(RpnExpr& pRpnExpr, const std::string& pMessage)
{
  m_CommandQueue.push_back(new AssertCmd(pRpnExpr, pMessage));
}

void ScriptFile::addAssignment(LinkerScript& pLDScript,
                               const std::string& pSymbolName,
                               RpnExpr& pRpnExpr,
                               Assignment::Type pType)
{
  Assignment* assignment =
    new Assignment(pLDScript, pType,
                   *(Operand::create(Operand::SYMBOL, pSymbolName)), pRpnExpr);

  if (m_InSectionsCmd) {
    assert(!m_CommandQueue.empty());
    SectionsCmd* sections = llvm::cast<SectionsCmd>(back());
    if (m_InOutputSectDesc) {
      assert(!sections->empty());
      OutputSectDesc* output_desc =
        llvm::cast<OutputSectDesc>(sections->back());
      output_desc->push_back(assignment);
    } else {
      sections->push_back(assignment);
    }
  } else {
    m_CommandQueue.push_back(assignment);
  }
}

bool ScriptFile::hasSectionsCmd() const
{
  return m_HasSectionsCmd;
}

void ScriptFile::enterSectionsCmd()
{
  m_HasSectionsCmd = true;
  m_InSectionsCmd = true;
  m_CommandQueue.push_back(new SectionsCmd());
}

void ScriptFile::leaveSectionsCmd()
{
  m_InSectionsCmd = false;
}

void ScriptFile::enterOutputSectDesc(const std::string& pName,
                                     const OutputSectDesc::Prolog& pProlog)
{
  assert(!m_CommandQueue.empty());
  assert(m_InSectionsCmd);
  SectionsCmd* sections = llvm::cast<SectionsCmd>(back());
  sections->push_back(new OutputSectDesc(pName, pProlog));

  m_InOutputSectDesc = true;
}

void ScriptFile::leaveOutputSectDesc(const OutputSectDesc::Epilog& pEpilog)
{
  assert(!m_CommandQueue.empty());
  assert(m_InSectionsCmd);
  SectionsCmd* sections = llvm::cast<SectionsCmd>(back());

  assert(!sections->empty() && m_InOutputSectDesc);
  OutputSectDesc* output_sect =  llvm::cast<OutputSectDesc>(sections->back());
  output_sect->setEpilog(pEpilog);

  m_InOutputSectDesc = false;
}

void ScriptFile::addInputSectDesc(InputSectDesc::KeepPolicy pPolicy,
                                  const InputSectDesc::Spec& pSpec)
{
  assert(!m_CommandQueue.empty());
  assert(m_InSectionsCmd);
  SectionsCmd* sections = llvm::cast<SectionsCmd>(back());

  assert(!sections->empty() && m_InOutputSectDesc);
  OutputSectDesc* output_sect =  llvm::cast<OutputSectDesc>(sections->back());

  output_sect->push_back(new InputSectDesc(pPolicy, pSpec));
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

