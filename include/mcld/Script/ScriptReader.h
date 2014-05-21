//===- ScriptReader.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_SCRIPTREADER_H
#define MCLD_SCRIPT_SCRIPTREADER_H

#include <mcld/LD/LDReader.h>

namespace mcld {

class Module;
class ScriptFile;
class Input;
class GroupReader;
class LinkerConfig;
class LinkerScript;
class TargetLDBackend;

class ScriptReader : public LDReader
{
public:
  ScriptReader(GroupReader& pGroupReader);

  ~ScriptReader();

  /// readScript
  bool readScript(const LinkerConfig& pConfig, ScriptFile& pScriptFile);

  /// isMyFormat
  bool isMyFormat(Input& pInput, bool &pContinue) const;

  GroupReader& getGroupReader() { return m_GroupReader; }

private:
  GroupReader& m_GroupReader;
};

} // namespace of mcld

#endif

