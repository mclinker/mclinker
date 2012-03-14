//===- PLT.h --------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef PROCEDURE_LINKAGE_TABLE_H
#define PROCEDURE_LINKAGE_TABLE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/LDSection.h>
#include <mcld/MC/MCTargetFragment.h>
#include <llvm/ADT/ilist.h>

namespace mcld
{

class ResolveInfo;

/** \class PLTEntry
 */
class PLTEntry : public MCTargetFragment
{
public:
  PLTEntry(size_t pSize, llvm::MCSectionData* pParent);
  virtual ~PLTEntry();

  size_t getEntrySize() const
  { return m_EntrySize; }

  void setContent(unsigned char* pContent)
  { m_pContent = pContent; }

  const unsigned char* getContent() const
  { return m_pContent; }

  //Used by llvm::cast<>.
  static bool classof(const MCFragment *O)
  { return true; }

  size_t getSize() const
  { return m_EntrySize; }

protected:
  size_t m_EntrySize;
  unsigned char* m_pContent;
};

/** \class PLT
 *  \brief Procedure linkage table
 */
class PLT
{
public:
  PLT(LDSection& pSection, llvm::MCSectionData& pSectionData);
  virtual ~PLT();

  const LDSection& getSection() const
  { return m_Section; }

  const llvm::MCSectionData& getSectionData() const
  { return m_SectionData; }

public:
  /// reserveEntry - reseve the number of pNum of empty entries
  /// The empty entris are reserved for layout to adjust the fragment offset.
  virtual void reserveEntry(size_t pNum = 1) = 0;

  /// getPLTEntry - get an empty entry or an exitsted filled entry with pSymbol.
  /// @param pSymbol - the target symbol
  /// @param pExist - ture if the a filled entry with pSymbol existed, otherwise false.
  virtual PLTEntry* getPLTEntry(const ResolveInfo& pSymbol, bool& pExist) = 0;

protected:
  LDSection& m_Section;
  llvm::MCSectionData& m_SectionData;
};

} // namespace of mcld

#endif

