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
InputSectDesc::InputSectDesc(KeepPolicy pPolicy, const Spec& pSpec)
  : ScriptCommand(ScriptCommand::InputSectDesc),
    m_KeepPolicy(pPolicy),
    m_pWildcardFile(pSpec.file),
    m_pExcludeFiles(pSpec.exclude_files),
    m_pWildcardSections(pSpec.wildcard_sections)
{
}

InputSectDesc::~InputSectDesc()
{
}

bool InputSectDesc::hasFile() const
{
  return m_pWildcardFile != NULL;
}

const WildcardPattern& InputSectDesc::file() const
{
  assert(hasFile());
  return *m_pWildcardFile;
}

bool InputSectDesc::hasExcludeFiles() const
{
  return m_pExcludeFiles != NULL && !m_pExcludeFiles->empty();
}

const ScriptInput& InputSectDesc::excludeFiles() const
{
  assert(hasExcludeFiles());
  return *m_pExcludeFiles;
}

bool InputSectDesc::hasSections() const
{
  return m_pWildcardSections != NULL && !m_pWildcardSections->empty();
}

const ScriptInput& InputSectDesc::sections() const
{
  assert(hasSections());
  return *m_pWildcardSections;
}

void InputSectDesc::dump() const
{
  if (m_KeepPolicy == Keep)
    mcld::outs() << "KEEP (";

  assert (hasFile());
  if (file().sortPolicy() == WildcardPattern::SORT_BY_NAME)
    mcld::outs() << "SORT (";

  mcld::outs() << file().name();

  if (hasSections()) {
    mcld::outs() << "(";

    if (hasExcludeFiles()) {
      mcld::outs() << "EXCLUDE_FILE (";
      for (ScriptInput::const_iterator it = excludeFiles().begin(),
        ie = excludeFiles().end(); it != ie; ++it) {
        mcld::outs() << (*it)->name() << " ";
      }
      mcld::outs() << ")";
    }

    if (hasSections()) {
      for (ScriptInput::const_iterator it = sections().begin(),
        ie = sections().end(); it != ie; ++it) {
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
    }
    mcld::outs() << ")";
  }

  if (file().sortPolicy() == WildcardPattern::SORT_BY_NAME)
    mcld::outs() << ")";

  if (m_KeepPolicy == Keep)
    mcld::outs() << ")";

  mcld::outs() << "\n";
}

void InputSectDesc::activate()
{
  // TODO
}
