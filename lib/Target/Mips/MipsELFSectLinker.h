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
                    const llvm::cl::opt<std::string> &pInputFilename,
                    const std::string &pOutputFilename,
                    unsigned int pOutputLinkType,
                    mcld::TargetLDBackend &pLDBackend);

  ~MipsELFSectLinker();
};

} // namespace of mcld

#endif

