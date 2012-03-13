//===- X86PLT.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_PLT_H
#define X86_PLT_H

#include <mcld/Target/PLT.h>

namespace mcld {

class X86GOT;
class GOTEntry;
class Output;

class X86PLT0 : public PLTEntry {
public:
  X86PLT0(llvm::MCSectionData* pParent, unsigned int pSize);
};

class X86PLT1 : public PLTEntry {
public:
  X86PLT1(llvm::MCSectionData* pParent, unsigned int pSize);
};

/** \class X86PLT
 *  \brief X86 Procedure Linkage Table
 */
class X86PLT : public PLT
{
  typedef llvm::DenseMap<const ResolveInfo*, X86PLT1*> SymbolIndexType;

public:
  typedef llvm::MCSectionData::iterator iterator;
  typedef llvm::MCSectionData::const_iterator const_iterator;

public:
  X86PLT(LDSection& pSection,
         llvm::MCSectionData& pSectionData,
         X86GOT& pGOTPLT,
	 const Output& pOutput);
  ~X86PLT();

// Override virtual function.
public:

  // reserveEntry is X86GOT friend function.
  void reserveEntry(size_t pNum = 1) ;

  PLTEntry* getPLTEntry(const ResolveInfo& pSymbol, bool& pExist) ;

  GOTEntry* getGOTPLTEntry(const ResolveInfo& pSymbol, bool& pExist);

public:

  iterator begin() { return m_SectionData.begin(); }

  const_iterator begin() const { return m_SectionData.begin(); }

  iterator end() { return m_SectionData.end(); }

  const_iterator end() const { return m_SectionData.end(); }

  X86PLT0* getPLT0() const;

  void applyPLT0();

  void applyPLT1();

private:
  X86GOT& m_GOT;

  // Used by getEntry() for mapping a ResolveInfo
  // instance to a PLT1 Entry.
  iterator m_PLTEntryIterator;

  SymbolIndexType m_PLTEntryMap;

  const uint8_t *m_PLT0;
  const uint8_t *m_PLT1;
  unsigned int m_PLT0Size;
  unsigned int m_PLT1Size;
};

} // namespace of mcld

#endif
