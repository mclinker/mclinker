//===- ARMAndroidSectLinker.h ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef ARM_ANDROIDSECTLINKER_H
#define ARM_ANDROIDSECTLINKER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/AndroidSectLinker.h>

namespace mcld
{

class MCLDInfo;

/** \class ARMAndroidSectLinker
 *  \brief ARMAndroidSectLinker sets up the environment for linking.
 *
 *  \see
 *  \author Anders Cheng <Anders.Cheng@mediatek.com>
 */
class ARMAndroidSectLinker : public AndroidSectLinker
{
public:
  ARMAndroidSectLinker(const std::string &pInputFilename,
                       const std::string &pOutputFilename,
                       unsigned int pOutputLinkType,
                       MCLDInfo& pLDInfo,
                       mcld::TargetLDBackend &pLDBackend);

  ~ARMAndroidSectLinker();
};

} // namespace of mcld

#endif

