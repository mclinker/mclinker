//===- ELFSegment.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_SEGMENT_H
#define MCLD_ELF_SEGMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Support/ELF.h>
#include <llvm/Support/DataTypes.h>
#include <mcld/LD/LDSection.h>
#include <cassert>
#include <vector>

namespace mcld
{

/** \class ELFSegment
 *  \brief decribe the program header for ELF executable or shared object
 */
class ELFSegment
{
public:
  typedef std::vector<LDSection*>::iterator sect_iterator;
  typedef std::vector<LDSection*>::const_iterator const_sect_iterator;
public:
  ELFSegment(uint32_t pType,
             uint32_t pFlag = llvm::ELF::PF_R,
             uint64_t pOffset = 0,
             uint64_t pVaddr = 0,
             uint64_t pPaddr = 0,
             uint64_t pFilesz = 0,
             uint64_t pMemsz = 0,
             uint64_t pAlign = 0,
             uint64_t pMaxSectAlign = 0);
  ~ELFSegment();

  ///  -----  iterators  -----  ///
  sect_iterator sectBegin()
  { return m_SectionList.begin(); }

  sect_iterator sectEnd()
  { return m_SectionList.end(); }

  const_sect_iterator sectBegin() const
  { return m_SectionList.begin(); }

  const_sect_iterator sectEnd() const
  { return m_SectionList.end(); }

  LDSection* getFirstSection()
  {
    if (0 == numOfSections())
      return NULL;
    return m_SectionList[0];
  }

  LDSection* getLastSection()
  {
    if (0 == numOfSections())
      return NULL;
    return m_SectionList[numOfSections() - 1];
  }

  const LDSection* getFirstSection() const
  {
    if (0 == numOfSections())
      return NULL;
    return m_SectionList[0];
  }

  const LDSection* getLastSection() const
  {
    if (0 == numOfSections())
      return NULL;
    return m_SectionList[numOfSections() - 1];
  }

  ///  -----  observers  -----  ///
  uint32_t type() const
  { return m_Type; }

  uint64_t offset() const
  { return m_Offset; }

  uint64_t vaddr() const
  { return m_Vaddr; }

  uint64_t paddr() const
  { return m_Paddr; }

  uint64_t filesz() const
  { return m_Filesz; }

  uint64_t memsz() const
  { return m_Memsz; }

  uint32_t flag() const
  { return m_Flag; }

  uint64_t align() const
  { return std::max(m_Align, m_MaxSectionAlign); }

  size_t numOfSections() const
  { return m_SectionList.size(); }

  ///  -----  modifiers  -----  ///
  void setOffset(uint64_t pOffset)
  { m_Offset = pOffset; }

  void setVaddr(uint64_t pVaddr)
  { m_Vaddr = pVaddr; }

  void setPaddr(uint64_t pPaddr)
  { m_Paddr = pPaddr; }

  void setFilesz(uint64_t pFilesz)
  { m_Filesz = pFilesz; }

  void setMemsz(uint64_t pMemsz)
  { m_Memsz = pMemsz; }

  void setFlag(uint32_t pFlag)
  { m_Flag = pFlag; }

  void updateFlag(uint32_t pFlag)
  {
    // PT_TLS segment should be PF_R
    if (llvm::ELF::PT_TLS != m_Type)
      m_Flag |= pFlag;
  }

  void setAlign(uint64_t pAlign)
  { m_Align = pAlign; }

  void addSection(LDSection* pSection)
  {
    assert(NULL != pSection);
    if (pSection->align() > m_MaxSectionAlign)
      m_MaxSectionAlign = pSection->align();
    m_SectionList.push_back(pSection);
  }

private:
  uint32_t m_Type;            // Type of segment
  uint32_t m_Flag;            // Segment flags
  uint64_t m_Offset;          // File offset where segment is located, in bytes
  uint64_t m_Vaddr;           // Virtual address of the segment
  uint64_t m_Paddr;           // Physical address of the segment (OS-specific)
  uint64_t m_Filesz;          // # of bytes in file image of segment (may be 0)
  uint64_t m_Memsz;           // # of bytes in mem image of segment (may be 0)
  uint64_t m_Align;           // alignment constraint
  uint64_t m_MaxSectionAlign; // max alignment of the sections in this segment
  std::vector<LDSection*> m_SectionList;
};

} // namespace of mcld

#endif

