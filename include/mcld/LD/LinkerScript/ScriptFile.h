//===- ScriptFile.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SCRIPT_FILE_INTERFACE_H
#define MCLD_SCRIPT_FILE_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/LinkerScript/ScriptInput.h>
#include <vector>
#include <string>

namespace mcld
{

class ScriptCommand;
class Input;
class InputTree;
class InputBuilder;
class GroupReader;
class LinkerConfig;
class LinkerScript;

/** \class ScriptFile
 *  \brief This class defines the interfaces to a linker script file.
 */

class ScriptFile
{
public:
  enum Kind {
    LDScript,      // -T
    Expression,    // --defsym
    VersionScript, // --version-script
    DynamicList,   // --dynamic-list
    Unknown,
  };

  struct ParserStrToken {
    const char* text;
    size_t length;
  };

  typedef std::vector<ScriptCommand*> CommandQueue;
  typedef CommandQueue::const_iterator const_iterator;
  typedef CommandQueue::iterator iterator;
  typedef CommandQueue::const_reference const_reference;
  typedef CommandQueue::reference reference;

public:
  ScriptFile(Kind pKind, Input& pInput, InputBuilder& pBuilder);
  ~ScriptFile();

  const_iterator   begin()  const { return m_CommandQueue.begin(); }
  iterator         begin()        { return m_CommandQueue.begin(); }
  const_iterator   end()    const { return m_CommandQueue.end(); }
  iterator         end()          { return m_CommandQueue.end(); }

  const_reference  front()  const { return m_CommandQueue.front(); }
  reference        front()        { return m_CommandQueue.front(); }
  const_reference  back()   const { return m_CommandQueue.back(); }
  reference        back()         { return m_CommandQueue.back(); }

  Kind getKind() const { return m_Kind; }

  const Input&     script() const { return m_Script; }
  Input&           script()       { return m_Script; }

  const InputTree& inputs() const { return *m_pInputTree; }
  InputTree&       inputs()       { return *m_pInputTree; }

  const std::string& name() const { return m_Name; }
  std::string&       name()       { return m_Name; }

  void dump() const;
  void activate();

  /// ENTRY(symbol)
  void addEntryPoint(const ParserStrToken& pSymbol, LinkerScript& pScript);

  /// OUTPUT_FORMAT(bfdname)
  /// OUTPUT_FORMAT(default, big, little)
  void addOutputFormatCmd(const ParserStrToken& pFormat);
  void addOutputFormatCmd(const ParserStrToken& pDefault,
                          const ParserStrToken& pBig,
                          const ParserStrToken& pLittle);

  void addScriptInput(const ParserStrToken& pPath);

  /// AS_NEEDED(file, file, ...)
  /// AS_NEEDED(file file ...)
  void setAsNeeded(bool pEnable = true);

  /// GROUP(file, file, ...)
  /// GROUP(file file ...)
  void addGroupCmd(GroupReader& pGroupReader, const LinkerConfig& pConfig);

  /// SEARCH_DIR(path)
  void addSearchDirCmd(const ParserStrToken& pPath, LinkerScript& pScript);

  /// OUTPUT_ARCH(bfdarch)
  void addOutputArchCmd(const ParserStrToken& pArch);

private:
  Kind m_Kind;
  std::string m_Name;
  Input& m_Script;
  InputTree* m_pInputTree;
  InputBuilder& m_Builder;
  CommandQueue m_CommandQueue;
};

} // namespace of mcld

#endif

