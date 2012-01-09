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

/** \class X86ELFSectLinker
 *  \brief X86ELFSectLinker sets up the environment for linking.
 *
 *  \see
 */
class X86ELFSectLinker : public SectLinker
{
public:
  X86ELFSectLinker(SectLinkerOption &pOption,
                   mcld::TargetLDBackend &pLDBackend);

  ~X86ELFSectLinker();
};

} // namespace of mcld

#endif

