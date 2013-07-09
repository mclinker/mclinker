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
class StringList;

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
    StringList* exclude_files;
    StringList* wildcard_sections;
  };

public:
  InputSectDesc(KeepPolicy pPolicy, const Spec& pSpec);
  ~InputSectDesc();

  KeepPolicy policy() const { return m_KeepPolicy; }

  bool hasFile() const;
  const WildcardPattern& file() const;

  bool hasExcludeFiles() const;
  const StringList& excludeFiles() const;

  bool hasSections() const;
  const StringList& sections() const;

  void dump() const;

  static bool classof(const ScriptCommand* pCmd)
  {
    return pCmd->getKind() == ScriptCommand::INPUT_SECT_DESC;
  }

  void activate();

private:
  KeepPolicy m_KeepPolicy;
  WildcardPattern* m_pWildcardFile;
  StringList* m_pExcludeFiles;
  StringList* m_pWildcardSections;
};

} // namespace of mcld

#endif
