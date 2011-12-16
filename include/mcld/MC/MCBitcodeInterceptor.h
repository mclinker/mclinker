//===- MCBitcodeInterceptor.h ---------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_BITCODE_INTERCEPTOR_H
#define MCLD_BITCODE_INTERCEPTOR_H
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

/** \class MCBitcodeInterceptor
 *  \brief MCBitcodeInterceptor converts bitcode into LDContext
 *
 *  @see LDContext
 *  @see MCObjectWriter
 */
class MCBitcodeInterceptor : public llvm::MCObjectWriter
{
public:
  MCBitcodeInterceptor(llvm::MCObjectStreamer&, TargetLDBackend&, MCLDInfo&);
  ~MCBitcodeInterceptor();

  void ExecutePostLayoutBinding(llvm::MCAssembler &Asm,
                                const llvm::MCAsmLayout &Layout);

  /// RecordRelocation - record relocations
  //  make a LDRelocation and recordds in the LDContext.
  void RecordRelocation(const llvm::MCAssembler &Asm,
                        const llvm::MCAsmLayout &Layout,
                        const llvm::MCFragment *Fragment,
                        const llvm::MCFixup &Fixup,
                        llvm::MCValue Target,
                        uint64_t &FixedValue);

  /// WriteObject - not really write out a object. Instead, load data to
  /// LDContext
  void WriteObject(llvm::MCAssembler &Asm, const llvm::MCAsmLayout &Layout);

private:
  TargetLDBackend& m_Backend;
  MCLDInfo& m_LDInfo;

};

} // namespace of mcld

#endif

