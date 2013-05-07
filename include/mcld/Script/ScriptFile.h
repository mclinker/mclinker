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

#include <mcld/Script/ScriptInput.h>
#include <mcld/Script/Assignment.h>
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
class ExprToken;

/** \class ScriptFile
 *  \brief This class defines the interfaces to a linker script file.
 */

class ScriptFile
{
public:
  enum Type {
    InputData,
    StringData
  };

  enum Kind {
    LDScript,      // -T
    Expression,    // --defsym
    VersionScript, // --version-script
    DynamicList,   // --dynamic-list
    Unknown
  };

  typedef std::vector<ScriptCommand*> CommandQueue;
  typedef CommandQueue::const_iterator const_iterator;
  typedef CommandQueue::iterator iterator;
  typedef CommandQueue::const_reference const_reference;
  typedef CommandQueue::reference reference;

public:
  ScriptFile(Kind pKind, Input& pInput, InputBuilder& pBuilder);
  ScriptFile(Kind pKind,
             const std::string& pName,
             std::string& pData,
             InputBuilder& pBuilder);
  ~ScriptFile();

  const_iterator   begin()  const { return m_CommandQueue.begin(); }
  iterator         begin()        { return m_CommandQueue.begin(); }
  const_iterator   end()    const { return m_CommandQueue.end(); }
  iterator         end()          { return m_CommandQueue.end(); }

  const_reference  front()  const { return m_CommandQueue.front(); }
  reference        front()        { return m_CommandQueue.front(); }
  const_reference  back()   const { return m_CommandQueue.back(); }
  reference        back()         { return m_CommandQueue.back(); }

  Type getType() const { return m_Type; }

  Kind getKind() const { return m_Kind; }

  const Input* inputData() const { return m_Data.input; }
  Input*       inputData()       { return m_Data.input; }

  const std::string* strData() const { return m_Data.str; }
  std::string*       strData()       { return m_Data.str; }

  const InputTree& inputs() const { return *m_pInputTree; }
  InputTree&       inputs()       { return *m_pInputTree; }

  const std::string& name() const { return m_Name; }
  std::string&       name()       { return m_Name; }

  void dump() const;
  void activate();

  /// ENTRY(symbol)
  void addEntryPoint(const std::string& pSymbol, LinkerScript& pScript);

  /// OUTPUT_FORMAT(bfdname)
  /// OUTPUT_FORMAT(default, big, little)
  void addOutputFormatCmd(const std::string& pFormat);
  void addOutputFormatCmd(const std::string& pDefault,
                          const std::string& pBig,
                          const std::string& pLittle);

  void addScriptInput(const std::string& pPath);

  /// AS_NEEDED(file, file, ...)
  /// AS_NEEDED(file file ...)
  void setAsNeeded(bool pEnable = true);

  /// GROUP(file, file, ...)
  /// GROUP(file file ...)
  void addGroupCmd(GroupReader& pGroupReader, const LinkerConfig& pConfig);

  /// SEARCH_DIR(path)
  void addSearchDirCmd(const std::string& pPath, LinkerScript& pScript);

  /// OUTPUT_ARCH(bfdarch)
  void addOutputArchCmd(const std::string& pArch);

  /// assignment
  void addAssignment(LinkerScript& pLDScript,
                     const std::string& pSymbol,
                     Assignment::Type pType = Assignment::DEFAULT);
  void addExprToken(ExprToken* pToken);


  static const std::string& createParserStr(const char* pText, size_t pLength);

  static void clearParserStrPool();

private:
  union ScriptData
  {
    Input* input;
    std::string* str;
  };

private:
  Type m_Type;
  Kind m_Kind;
  std::string m_Name;
  ScriptData m_Data;
  InputTree* m_pInputTree;
  InputBuilder& m_Builder;
  CommandQueue m_CommandQueue;
};

} // namespace of mcld

#endif

