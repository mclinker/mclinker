//===- LDSymbol.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_SYMBOL_H
#define MCLD_LD_SYMBOL_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/ADT/Uncopyable.h"
#include "mcld/LD/ResolveInfo.h"
#include "llvm/MC/MCAssembler.h"

namespace mcld
{

/** \class LDSymbol
 *  \brief LDSymbol provides a consistent abstraction for different formats
 *  in different targets.
 */
class LDSymbol
{
public:
  // FIXME: use SizeTrait<32> or SizeTrait<64> instead of big type
  typedef uint64_t SizeType;
  typedef ResolveInfo::ValueType ValueType;
  typedef MCFragmentRef::Offset Offset;

public:
  LDSymbol();
  LDSymbol(const LDSymbol& pCopy);
  LDSymbol& operator=(const LDSymbol& pCopy);
  ~LDSymbol();

  // -----  observers  ----- //
  const char* name() const
  { return m_pResolveInfo->name(); }

  llvm::StringRef str() const
  { return m_pResolveInfo->key(); }

  bool isDyn() const
  { return m_pResolveInfo->isDyn(); }

  unsigned int type() const
  { return m_pResolveInfo->type(); }

  unsigned int binding() const
  { return m_pResolveInfo->binding(); }

  uint8_t other() const
  { return m_pResolveInfo->other(); }

  uint8_t visibility() const
  { return m_pResolveInfo->other(); }

  const MCFragmentRef* fragRef() const
  { return m_pFragRef; }

  SizeType size() const
  { return m_Size; }

  ValueType value() const
  { return m_pResolveInfo->value(); }

  // -----  modifiers  ----- //
  void setSize(SizeType pSize)
  { m_Size = pSize; }

  void setFragmentRef(llvm::MCFragment& pFragment, Offset pOffset);

  void setResolveInfo(const ResolveInfo& pInfo);

private:
  // -----  Symbol's fields  ----- //
  ResolveInfo* m_pResolveInfo;
  MCFragmentRef* m_pFragRef;
  SizeType m_Size;

};

} // namespace mcld

#endif

