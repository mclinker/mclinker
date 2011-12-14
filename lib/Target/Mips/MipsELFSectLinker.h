//===- MipsELFSectLinker.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_ELFSECTLINKER_H
#define MIPS_ELFSECTLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/CodeGen/SectLinker.h"

namespace mcld
{

class MCLDInfo;

/** \class MipsELFSectLinker
 *  \brief MipsELFSectLinker sets up the environment for linking.
 *
 *  \see
 */
class MipsELFSectLinker : public SectLinker
{
public:
  MipsELFSectLinker(const std::string &pInputFilename,
                    const std::string &pOutputFilename,
                    unsigned int pOutputLinkType,
                    MCLDInfo& pLDInfo,
                    mcld::TargetLDBackend &pLDBackend);

  ~MipsELFSectLinker();
};

} // namespace of mcld

#endif

