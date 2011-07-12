/*****************************************************************************
 *   The MC Linker Project, Copyright (C), 2011 -                            *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
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
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class ARMELFSectLinker : public SectLinker
{
public:
  ARMELFSectLinker(TargetLDBackend &pLDBackend);
  ~ARMELFSectLinker();
};

} // namespace of mcld

#endif

