//===-  X86Relocator.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_RELOCATION_FACTORY_H
#define X86_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/Relocator.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/PLT.h>
#include <mcld/Target/SymbolEntryMap.h>
#include "X86LDBackend.h"

namespace mcld {

class ResolveInfo;

/** \class X86Relocator
 *  \brief X86Relocator creates and destroys the X86 relocations.
 *
 */
class X86Relocator : public Relocator
{
public:
  typedef SymbolEntryMap<PLTEntryBase> SymPLTMap;

public:
  X86Relocator();
  ~X86Relocator();

  virtual Result applyRelocation(Relocation& pRelocation) = 0;

  virtual const char* getName(Relocation::Type pType) const = 0;

  const SymPLTMap& getSymPLTMap() const { return m_SymPLTMap; }
  SymPLTMap&       getSymPLTMap()       { return m_SymPLTMap; }

private:
  SymPLTMap m_SymPLTMap;
};

/** \class X86_32Relocator
 *  \brief X86_32Relocator creates and destroys the X86-32 relocations.
 *
 */
class X86_32Relocator : public X86Relocator
{
public:
  typedef SymbolEntryMap<X86_32GOTEntry> SymGOTMap;
  typedef SymbolEntryMap<X86_32GOTEntry> SymGOTPLTMap;

  enum {
    R_386_TLS_OPT = 44 // mcld internal relocation type
  };

public:
  X86_32Relocator(X86_32GNULDBackend& pParent);

  Result applyRelocation(Relocation& pRelocation);

  X86_32GNULDBackend& getTarget()
  { return m_Target; }

  const X86_32GNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

  const SymGOTPLTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTPLTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

private:
  X86_32GNULDBackend& m_Target;
  SymGOTMap m_SymGOTMap;
  SymGOTPLTMap m_SymGOTPLTMap;
};

/** \class X86_64Relocator
 *  \brief X86_64Relocator creates and destroys the X86-64 relocations.
 *
 */
class X86_64Relocator : public X86Relocator
{
public:
  typedef SymbolEntryMap<X86_64GOTEntry> SymGOTMap;
  typedef SymbolEntryMap<X86_64GOTEntry> SymGOTPLTMap;

public:
  X86_64Relocator(X86_64GNULDBackend& pParent);

  Result applyRelocation(Relocation& pRelocation);

  X86_64GNULDBackend& getTarget()
  { return m_Target; }

  const X86_64GNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

  const SymGOTPLTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTPLTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

private:
  X86_64GNULDBackend& m_Target;
  SymGOTMap m_SymGOTMap;
  SymGOTPLTMap m_SymGOTPLTMap;
};

} // namespace of mcld

#endif

