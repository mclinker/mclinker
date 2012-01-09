//===- X86AndroidSectLinker.h ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef X86_ANDROIDSECTLINKER_H
#define X86_ANDROIDSECTLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/AndroidSectLinker.h>

namespace mcld
{

/** \class X86AndroidSectLinker
 *  \brief X86AndroidSectLinker sets up the environment for linking.
 *
 *  \see
 *  \author Anders Cheng <Anders.Cheng@mediatek.com>
 */
class X86AndroidSectLinker : public AndroidSectLinker
{
public:
  X86AndroidSectLinker(SectLinkerOption &pOption,
                       mcld::TargetLDBackend &pLDBackend);

  ~X86AndroidSectLinker();
};

} // namespace of mcld

#endif

