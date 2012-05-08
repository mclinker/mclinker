//===- ARMELFSectLinker.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_ELF_SECTION_LINKER_H
#define ARM_ELF_SECTION_LINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/CodeGen/SectLinker.h>

namespace mcld
{

/** \class ARMELFSectLinker
 *  \brief ARMELFSectLinker sets up the environment for linking.
 *
 *  \see
 */
class ARMELFSectLinker : public SectLinker
{
public:
  ARMELFSectLinker(SectLinkerOption &pOption,
                   mcld::TargetLDBackend &pLDBackend);

  ~ARMELFSectLinker();
};

} // namespace of mcld

#endif

