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
#include <mcld/MC/MCLDInput.h>
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
  Input* input = NULL;
  MemoryRegion* region = NULL;
  std::stringbuf buf;

  if (pScriptFile.getType() == ScriptFile::InputData) {
    input = pScriptFile.inputData();
    size_t size = input->memArea()->size();
    region = input->memArea()->request(input->fileOffset(), size);
    char* str = reinterpret_cast<char*>(region->getBuffer());
    buf.pubsetbuf(str, size);
  } else {
    // ScriptFile is from a string data
    buf.str(*(pScriptFile.strData()));
  }

  std::istream in(&buf);
  ScriptScanner scanner(&in);
  ScriptParser parser(pConfig, pLDScript, pScriptFile, scanner, m_GroupReader);

  result = (0 == parser.parse());;

  if (pScriptFile.getType() == ScriptFile::InputData)
    input->memArea()->release(region);

  return result;
}

