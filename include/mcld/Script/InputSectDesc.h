//===- InputSectDesc.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_INPUT_SECTION_DESCRIPTION_INTERFACE_H
#define MCLD_INPUT_SECTION_DESCRIPTION_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Script/ScriptCommand.h>

namespace mcld
{

class WildcardPattern;
class ScriptInput;

/** \class InputSectDesc
 *  \brief This class defines the interfaces to input section description.
 */

class InputSectDesc : public ScriptCommand
{
public:
  enum KeepPolicy {
    Keep,
    NoKeep
  };

  struct Spec {
    WildcardPattern* file;
    ScriptInput* exclude_files;
    ScriptInput* wildcard_sections;
  };

public:
  InputSectDesc(KeepPolicy pPolicy,
                WildcardPattern& pWildcardFile,
                ScriptInput& pExculdeFiles,
                ScriptInput& pWildcardSections);
  ~InputSectDesc();

  KeepPolicy policy() const { return m_KeepPolicy; }

  const WildcardPattern& file() const { return m_WildcardFile; }

  const ScriptInput& excludeFiles() const { return m_ExcludeFiles; }

  const ScriptInput& sections() const { return m_WildcardSections; }

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::InputSectDesc;
  }

  void activate();

private:
  KeepPolicy m_KeepPolicy;
  WildcardPattern& m_WildcardFile;
  ScriptInput& m_ExcludeFiles;
  ScriptInput& m_WildcardSections;
};

} // namespace of mcld

#endif
