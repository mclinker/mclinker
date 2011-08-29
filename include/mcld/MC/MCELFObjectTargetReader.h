/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_ELF_OBJECT_TARGET_READER_H
#define MCLD_ELF_OBJECT_TARGET_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <cstring>
#include <stdint.h>

namespace llvm {
  class MCValue;
  class MCSymbol;
  class MCFixup;
  class MCAssembler;
  class MCFragment;
}

namespace mcld
{
  using namespace llvm;

/** \class MCELFObjectTargetReader
 *  \brief MCELFObjectTargetReader provides an abstract ELF interface
 *         for target-dependent object readers.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class MCELFObjectTargetReader
{
protected:
  MCELFObjectTargetReader();

public:
  virtual ~MCELFObjectTargetReader();

  virtual bool hasRelocationAddend() = 0;

  virtual unsigned getRelocType(const MCValue&,
                                const MCFixup&,
                                bool IsPCRel,
                                bool IsRelocWithSymbol,
                                int64_t) = 0;

  virtual const MCSymbol* explicitRelSym(const MCAssembler&,
                                         const MCValue&,
                                         const MCFragment&,
                                         const MCFixup&,
                                         bool) const {
    return NULL;
  }
};

} // namespace of mcld

#endif
