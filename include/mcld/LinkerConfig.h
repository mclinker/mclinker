//===- LinkerConfig.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LINKER_CONFIG_H
#define MCLD_LINKER_CONFIG_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/Triple.h>

#include <mcld/GeneralOptions.h>
#include <mcld/ScriptOptions.h>
#include <mcld/BitcodeOption.h>
#include <mcld/AttributeOption.h>
#include <mcld/Support/Path.h>

#include <string>

namespace mcld {

/** \class LinkerConfig
 *  \brief LinkerConfig is composed of argumments of MCLinker.
 *   options()        - the general options
 *   scripts()        - the script options
 *   bitcode()        - the bitcode being linked
 *   attribute()      - the attribute options
 */
class LinkerConfig
{
public:
  enum CodeGenType {
    Unknown,
    Object,
    DynObj,
    Exec,
    External
  };

public:
  LinkerConfig();

  explicit LinkerConfig(const std::string &pTripleString);

  ~LinkerConfig();

  const GeneralOptions& options() const { return m_Options; }
  GeneralOptions&       options()       { return m_Options; }

  const ScriptOptions&  scripts() const { return m_Scripts; }
  ScriptOptions&        scripts()       { return m_Scripts; }

  const BitcodeOption&  bitcode() const { return m_Bitcode; }
  BitcodeOption&        bitcode()       { return m_Bitcode; }

  const AttributeOption& attribute() const { return m_Attribute; }
  AttributeOption&       attribute()       { return m_Attribute; }

  CodeGenType codeGenType() const { return m_CodeGenType; }

  void setCodeGenType(CodeGenType pType) { m_CodeGenType = pType; }

  const llvm::Triple& triple() const { return m_Triple; }

  void setTriple(const std::string& pTriple);

  void setTriple(const llvm::Triple& pTriple);

  static const char* version();

private:
  // -----  General Options  ----- //
  GeneralOptions m_Options;
  ScriptOptions m_Scripts;
  BitcodeOption m_Bitcode;
  AttributeOption m_Attribute;

  llvm::Triple m_Triple;
  CodeGenType m_CodeGenType;
};

} // namespace of mcld

#endif

