/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Wen-Han Gu <nowar100@mediatek.com>                                      *
 ****************************************************************************/
#ifndef MCLD_ASMOBJECT_READER_H
#define MCLD_ASMOBJECT_READER_H
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

/** \class MCAsmObjectReader
 *  \brief MCAsmObjectReader records all MCSectionData while writing data in
 *   MCAssembler.
 *
 *  @see MCObjectWriter
 */
class MCAsmObjectReader : public llvm::MCObjectWriter
{
public:
  MCAsmObjectReader(llvm::MCObjectStreamer&, TargetLDBackend&, MCLDInfo&);
  ~MCAsmObjectReader();

  void ExecutePostLayoutBinding(llvm::MCAssembler &Asm,
                                const llvm::MCAsmLayout &Layout);

  /// RecordRelocation - record relocations
  /// When a 
  void RecordRelocation(const llvm::MCAssembler &Asm,
                        const llvm::MCAsmLayout &Layout,
                        const llvm::MCFragment *Fragment,
                        const llvm::MCFixup &Fixup,
                        llvm::MCValue Target,
                        uint64_t &FixedValue);

  void WriteObject(llvm::MCAssembler &Asm, const llvm::MCAsmLayout &Layout);

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_LDInfo;

};

} // namespace of mcld

#endif

