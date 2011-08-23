/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                          *
 ****************************************************************************/
#ifndef MCLD_OBJECT_READER_H
#define MCLD_OBJECT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/MC/MCLDInput.h>
#include <llvm/Support/system_error.h>

namespace mcld
{
class MCObjectTargetReader;

//MCObjectReader provides an interface for different object formats.
class MCObjectReader
{
protected:
  MCObjectReader();
public:
  virtual ~MCObjectReader();

  virtual bool isMyFormat(mcld::Input &pFile) const = 0;
  virtual Input::Type fileType(mcld::Input &pFile) const = 0;
  virtual llvm::error_code readObject(const std::string &ObjectFile, 
                                      MCLDFile &) = 0;

  virtual bool hasRelocationAddend() = 0;
  virtual unsigned getRelocType(const MCValue& Target,
                                const MCFixup& Fixup,
                                bool IsPCRel,
                                bool IsRelocWithSymbol,
                                int64_t Addend) = 0;
  virtual const MCSymbol* explicitRelSym(const MCAssembler& Asm,
                                         const MCValue& Target,
                                         const MCFragment& F,
                                         const MCFixup& Fixup,
                                         bool IsPCRel) const = 0;
};

} // namespace of mcld

#endif

