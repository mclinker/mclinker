//===- X86ELFSectLinker.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_ELFSECTLINKER_H
#define X86_ELFSECTLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/CodeGen/SectLinker.h>

namespace mcld
{

class MCLDInfo;

/** \class X86ELFSectLinker
 *  \brief X86ELFSectLinker sets up the environment for linking.
 *
 *  \see
 */
class X86ELFSectLinker : public SectLinker
{
public:
  X86ELFSectLinker(const llvm::cl::opt<std::string> &pInputFilename,
                   const std::string &pOutputFilename,
                   unsigned int pOutputLinkType,
                   MCLDInfo& pLDInfo,
                   mcld::TargetLDBackend &pLDBackend);

  ~X86ELFSectLinker();
};

} // namespace of mcld

#endif

