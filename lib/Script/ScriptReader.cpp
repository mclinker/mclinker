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
#include <mcld/Script/ScriptCommand.h>
#include <mcld/Script/ScriptFile.h>
#include <mcld/MC/Input.h>
#include <mcld/LD/GroupReader.h>
#include <mcld/LinkerConfig.h>
#include <mcld/LinkerScript.h>
#include <mcld/Support/FileHandle.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>

#include <istream>
#include <sstream>
#include <string>
#include <cassert>

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
  std::stringbuf buf;
  Input& input = pScriptFile.input();
  size_t size = input.memArea()->size();
  MemoryRegion* region = input.memArea()->request(input.fileOffset(), size);
  char* str = reinterpret_cast<char*>(region->getBuffer());
  buf.pubsetbuf(str, size);

  std::istream in(&buf);
  ScriptScanner scanner(&in);
  ScriptParser parser(pConfig,
                      pScriptFile,
                      scanner,
                      m_GroupReader);
  result = (0 == parser.parse());;

  input.memArea()->release(region);
  return result;
}

