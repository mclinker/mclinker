/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCASMOBJECTREADER_H
#define MCASMOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/MC/MCObjectWriter.h>


namespace llvm
{
  class MCStreamer;
  class MCObjectStreamer;
  class MCAsmLayout;
  class MCAssembler;
  class MCFixup;
  class MCFragment;
  class MCSymbol;
  class MCSymbolData;
  class MCSymbolRefExpr;
  class MCValue;
  class raw_ostream;
} // namespace of llvm

namespace mcld
{

  class MCLDInfo;
  class TargetLDBackend;
  using namespace llvm;

/** \class MCAsmObjectReader
 *  \brief MCAsmObjectReader records all MCSectionData while writing data in
 *   MCAssembler.
 *
 *  @see MCObjectWriter
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class MCAsmObjectReader : public llvm::MCObjectWriter
{
public:
  MCAsmObjectReader(MCObjectStreamer&, TargetLDBackend&, MCLDInfo&);
  ~MCAsmObjectReader();

  void ExecutePostLayoutBinding(MCAssembler &Asm,
                                const MCAsmLayout &Layout);

  void RecordRelocation(const MCAssembler &Asm,
                        const MCAsmLayout &Layout,
                        const MCFragment *Fragment,
                        const MCFixup &Fixup, MCValue Target,
                        uint64_t &FixedValue);
  bool
    IsSymbolRefDifferenceFullyResolvedImpl(const MCAssembler &Asm,
                                           const MCSymbolData &DataA,
                                           const MCFragment &FB,
                                           bool InSet,
                                           bool IsPCRel) const;


  void WriteObject(MCAssembler &Asm, const MCAsmLayout &Layout);

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_LDInfo;
};

} // namespace of mcld

#endif

