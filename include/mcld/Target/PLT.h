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
  PLTEntry(const unsigned int size);
  virtual ~PLTEntry();

  unsigned int getEntrySize() const {
    return m_EntrySize;
  }

  void setContent(unsigned char* pContent) {
    m_pContent = pContent;
  }

  unsigned char* getContent() const {
    return m_pContent;
  }

  //Used by llvm::cast<>.
  static bool classof(const MCFragment *O) { return true; }

  /// offset - byte offset to the section
  uint64_t offset() const {
    return Offset;
  }

  uint64_t getSize() const {
    return m_EntrySize;
  }

protected:
  const uint64_t m_EntrySize;
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
  { return *m_pSection; }

  const llvm::MCSectionData& getSectionData() const
  { return *m_pSectionData; }

public:
  /// reserveEntry - reseve the number of pNum of empty entries
  /// The empty entris are reserved for layout to adjust the fragment offset.
  virtual void reserveEntry(int pNum = 1) = 0;

  /// getEntry - get an empty entry or an exitsted filled entry with pSymbol.
  /// @param pSymbol - the target symbol
  /// @param pExist - ture if the a filled entry with pSymbol existed, otherwise false.
  virtual PLTEntry* getEntry(const ResolveInfo& pSymbol, bool& pExist) = 0;

protected:
  LDSection* m_pSection;
  llvm::MCSectionData* m_pSectionData;
};

} // namespace of mcld

#endif
