//===- ELFSectLinker.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// ELFSectLinker is a customized linker pass for ELF platform.
// This pass set up default parameters for ELF.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_SECTION_LINKER_H
#define MCLD_ELF_SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/CodeGen/SectLinker.h>

namespace mcld
{

class ELFSectLinker : public SectLinker
{
public:
  ELFSectLinker(SectLinkerOption &pOption,
                mcld::TargetLDBackend &pLDBackend);

  virtual ~ELFSectLinker();
};

} // namespace of mcld

#endif

