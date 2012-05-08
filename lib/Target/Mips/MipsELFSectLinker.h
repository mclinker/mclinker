//===- MipsELFSectLinker.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_ELF_SECTION_LINKER_H
#define MIPS_ELF_SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/CodeGen/SectLinker.h>

namespace mcld
{

/** \class MipsELFSectLinker
 *  \brief MipsELFSectLinker sets up the environment for linking.
 */
class MipsELFSectLinker : public SectLinker
{
public:
  MipsELFSectLinker(SectLinkerOption &pOption,
                    mcld::TargetLDBackend &pLDBackend);

  ~MipsELFSectLinker();
};

} // namespace of mcld

#endif

