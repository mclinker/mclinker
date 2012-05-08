//===- ELFSectLinker.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/ELFSectLinker.h>
#include <mcld/CodeGen/SectLinkerOption.h>

using namespace mcld;

//==========================
// ELFSectLinker
ELFSectLinker::ELFSectLinker(SectLinkerOption &pOption,
                             TargetLDBackend &pLDBackend)
  : SectLinker(pOption, pLDBackend) {
}

ELFSectLinker::~ELFSectLinker()
{
  // SectLinker will delete m_pLDBackend and m_pLDDriver;
}

