//===- ScriptReader.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/LinkerScript/ScriptReader.h>

#include <mcld/LD/LinkerScript/ScriptScanner.h>
#include <mcld/LD/LinkerScript/ScriptCommand.h>
#include <mcld/LD/LinkerScript/ScriptFile.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/LD/GroupReader.h>
#include <mcld/LinkerConfig.h>

#include <fstream>
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
bool ScriptReader::isMyFormat(Input& input) const
{
  // always return true now
  return true;
}

bool ScriptReader::readScript(const LinkerConfig& pConfig,
                              ScriptFile& pScriptFile)
{
  std::ifstream in(pScriptFile.script().path().native().c_str());
  if (!in.good())
    return false;

  ScriptScanner scanner(&in);
  ScriptParser parser(pConfig, pScriptFile, scanner, *this);

  return (parser.parse() == 0);
}

