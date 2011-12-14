/*****************************************************************************
 *   The MC Linker Project, Copyright (C), 2011 -                            *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Anders Cheng <Anders.Cheng@mediatek.com>                                *
 ****************************************************************************/
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

