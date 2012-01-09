//===- MipsAndroidSectLinker.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_ANDROIDSECTLINKER_H
#define MIPS_ANDROIDSECTLINKER_H

#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/Target/AndroidSectLinker.h"

namespace mcld
{

/** \class MipsAndroidSectLinker
 *  \brief MipsAndroidSectLinker sets up the environment for linking.
 *
 */
class MipsAndroidSectLinker : public AndroidSectLinker
{
public:
  MipsAndroidSectLinker(SectLinkerOption &pOption,
                        mcld::TargetLDBackend &pLDBackend);

  ~MipsAndroidSectLinker();
};

} // namespace of mcld

#endif
