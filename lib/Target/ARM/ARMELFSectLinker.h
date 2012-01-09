//===- ARMELFSectLinker.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_ELFSECTLINKER_H
#define ARM_ELFSECTLINKER_H
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

