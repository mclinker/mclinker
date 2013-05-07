//===- ScriptReader.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_READER_H
#define MCLD_SCRIPT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/LDReader.h>
#include <string>

namespace mcld {

class ScriptFile;
class Input;
class GroupReader;
class LinkerConfig;
class LinkerScript;

class ScriptReader : public LDReader
{
public:
  ScriptReader(GroupReader& pGroupReader);

  ~ScriptReader();

  /// readScript
  bool readScript(const LinkerConfig& pConfig,
                  LinkerScript& pScript,
                  ScriptFile& pScriptFile);

  /// isMyFormat
  bool isMyFormat(Input& pInput) const;

  GroupReader& getGroupReader() { return m_GroupReader; }

private:
  GroupReader& m_GroupReader;
};

} // namespace of mcld

#endif

