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
#include "mcld/MC/MCFragmentRef.h"
#include "llvm/MC/MCAssembler.h"
#include <assert.h>

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
  typedef ResolveInfo::SizeType SizeType;
  typedef ResolveInfo::ValueType ValueType;
  typedef MCFragmentRef::Offset Offset;

public:
  LDSymbol();
  LDSymbol(const LDSymbol& pCopy);
  LDSymbol& operator=(const LDSymbol& pCopy);
  ~LDSymbol();

  // -----  observers  ----- //
  const char* name() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->name();
  }

  unsigned int nameSize() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->nameSize();
  }

  llvm::StringRef str() const {
    assert(NULL != m_pResolveInfo);
    return llvm::StringRef(m_pResolveInfo->name(), m_pResolveInfo->nameSize());
  }

  bool isDyn() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->isDyn();
  }

  unsigned int type() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->type();
  }

  unsigned int desc() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->desc();
  }
  unsigned int binding() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->binding();
  }

  uint8_t other() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->other();
  }

  uint8_t visibility() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->other();
  }

  ValueType value() const {
    assert(NULL != m_pResolveInfo);
    return m_pResolveInfo->value();
  }

  const MCFragmentRef* fragRef() const
  { return m_pFragRef; }

  SizeType size() const
  { return m_pResolveInfo->size(); }

  const ResolveInfo* resolveInfo() const 
  { return m_pResolveInfo; }
    
  // -----  modifiers  ----- //
  void setSize(SizeType pSize) {
    assert(NULL != m_pResolveInfo);
    m_pResolveInfo->setSize(pSize);
  }

  void setValue(ValueType pValue) {
    assert(NULL != m_pResolveInfo);
    m_pResolveInfo->setValue(pValue);
  }

  void setFragmentRef(llvm::MCFragment& pFragment, Offset pOffset);

  void setResolveInfo(const ResolveInfo& pInfo);

private:
  // -----  Symbol's fields  ----- //
  ResolveInfo* m_pResolveInfo;
  MCFragmentRef* m_pFragRef;

};

} // namespace mcld

#endif

