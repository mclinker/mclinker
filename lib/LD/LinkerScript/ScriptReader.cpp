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
#include <mcld/LinkerScript.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>

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
                              LinkerScript& pLDScript,
                              ScriptFile& pScriptFile)
{
  bool result = false;
  std::stringbuf buf;

  Input& input = pScriptFile.script();
  size_t size = input.memArea()->size();
  MemoryRegion* region = input.memArea()->request(input.fileOffset(), size);
  char* str = reinterpret_cast<char*>(region->getBuffer());

  buf.pubsetbuf(str, size);
  std::istream in(&buf);

  ScriptScanner scanner(&in);
  ScriptParser parser(pConfig, pLDScript, pScriptFile, scanner, *this);

  result = (parser.parse() == 0);

  input.memArea()->release(region);
  return result;
}

