//===- ScriptReader.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/ScriptReader.h>
#include <mcld/Script/ScriptScanner.h>
#include <mcld/Script/ScriptFile.h>
#include <mcld/MC/Input.h>
#include <mcld/Support/MemoryArea.h>

#include <llvm/ADT/StringRef.h>

#include <istream>
#include <sstream>

using namespace mcld;

ScriptReader::ScriptReader(GroupReader& pGroupReader)
  : m_GroupReader(pGroupReader)
{
}

ScriptReader::~ScriptReader()
{
}

/// isMyFormat
bool ScriptReader::isMyFormat(Input& input, bool &doContinue) const
{
  doContinue = true;
  // always return true now
  return true;
}

bool ScriptReader::readScript(const LinkerConfig& pConfig,
                              ScriptFile& pScriptFile)
{
  bool result = false;
  Input& input = pScriptFile.input();
  size_t size = input.memArea()->size();
  llvm::StringRef region = input.memArea()->request(input.fileOffset(), size);
  std::stringbuf buf(region.data());

  std::istream in(&buf);
  ScriptScanner scanner(&in);
  ScriptParser parser(pConfig,
                      pScriptFile,
                      scanner,
                      m_GroupReader);
  result = (0 == parser.parse());;

  return result;
}

