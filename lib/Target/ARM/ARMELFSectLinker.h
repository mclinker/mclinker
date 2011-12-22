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

class MCLDInfo;

/** \class ARMELFSectLinker
 *  \brief ARMELFSectLinker sets up the environment for linking.
 *
 *  \see
 */
class ARMELFSectLinker : public SectLinker
{
public:
  ARMELFSectLinker(const llvm::cl::opt<std::string> &pInputFilename,
                   const std::string &pOutputFilename,
                   unsigned int pOutputLinkType,
                   MCLDInfo& pLDInfo,
                   mcld::TargetLDBackend &pLDBackend);

  ~ARMELFSectLinker();
};

} // namespace of mcld

#endif

