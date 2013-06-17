//===- InputSectDesc.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Script/InputSectDesc.h>
#include <mcld/Script/ScriptInput.h>
#include <mcld/Script/WildcardPattern.h>
#include <mcld/Support/raw_ostream.h>
#include <llvm/Support/Casting.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// InputSectDesc
//===----------------------------------------------------------------------===//
InputSectDesc::InputSectDesc(KeepPolicy pPolicy,
                             WildcardPattern& pWildcardFile,
                             ScriptInput& pExculdeFiles,
                             ScriptInput& pWildcardSections)
  : ScriptCommand(ScriptCommand::InputSectDesc),
    m_KeepPolicy(pPolicy),
    m_WildcardFile(pWildcardFile),
    m_ExcludeFiles(pExculdeFiles),
    m_WildcardSections(pWildcardSections)
{
}

InputSectDesc::~InputSectDesc()
{
}

void InputSectDesc::dump() const
{
  if (m_KeepPolicy == Keep)
    mcld::outs() << "KEEP (";

  if (m_WildcardFile.sortPolicy() == WildcardPattern::SORT_BY_NAME)
    mcld::outs() << "SORT (";

  mcld::outs() << m_WildcardFile.name();

  if (!m_WildcardSections.empty()) {
    mcld::outs() << "(";

    if (!m_ExcludeFiles.empty()) {
      mcld::outs() << "EXCLUDE_FILE (";
      for (ScriptInput::const_iterator it = m_ExcludeFiles.begin(),
        ie = m_ExcludeFiles.end(); it != ie; ++it) {
        mcld::outs() << (*it)->name() << " ";
      }
      mcld::outs() << ")";
    }

    for (ScriptInput::const_iterator it = m_WildcardSections.begin(),
      ie = m_WildcardSections.end(); it != ie; ++it) {
      assert((*it)->kind() == StrToken::Wildcard);
      WildcardPattern* wildcard = llvm::cast<WildcardPattern>(*it);

      switch (wildcard->sortPolicy()) {
      case WildcardPattern::SORT_BY_NAME:
        mcld::outs() << "SORT (";
        break;
      case WildcardPattern::SORT_BY_ALIGNMENT:
        mcld::outs() << "SORT_BY_ALIGNMENT (";
        break;
      case WildcardPattern::SORT_BY_NAME_ALIGNMENT:
        mcld::outs() << "SORT_BY_NAME_ALIGNMENT (";
        break;
      case WildcardPattern::SORT_BY_ALIGNMENT_NAME:
        mcld::outs() << "SORT_BY_ALIGNMENT_NAME (";
        break;
      default:
        break;
      }

      mcld::outs() << wildcard->name() << " ";

      if (wildcard->sortPolicy() != WildcardPattern::SORT_NONE)
        mcld::outs() << ")";
    }

    mcld::outs() << ")";
  }

  if (m_WildcardFile.sortPolicy() == WildcardPattern::SORT_BY_NAME)
    mcld::outs() << ")";

  if (m_KeepPolicy == Keep)
    mcld::outs() << ")";

  mcld::outs() << "\n";
}

void InputSectDesc::activate()
{
  // TODO
}
