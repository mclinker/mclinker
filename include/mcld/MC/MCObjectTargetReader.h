/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef MCOBJECTTARGETREADER_H_
#define MCOBJECTTARGETREADER_H_
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
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

/** \class MCObjectTargetReader
 *  \brief MCObjectTargetReader provides an abstract interface for target-dependent object readers.
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
class MCObjectTargetReader
{
protected:
  MCObjectTargetReader() {}
public:
  virtual ~MCObjectTargetReader() {}

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
                                         bool) const = 0;
};

} // namespace of mcld

#endif
