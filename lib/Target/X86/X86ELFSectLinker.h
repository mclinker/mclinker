/*****************************************************************************
 *   The MC Linker Project, Copyright (C), 2011 -                            *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
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
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class X86ELFSectLinker : public SectLinker
{
public:
  X86ELFSectLinker(const std::string &pInputFilename,
                   const std::string &pOutputFilename,
                   unsigned int pOutputLinkType,
                   MCLDInfo& pLDInfo,
                   mcld::TargetLDBackend &pLDBackend);

  ~X86ELFSectLinker();
};

} // namespace of mcld

#endif

