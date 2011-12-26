//===- ELFDynObjWriter.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_DYNAMIC_SHARED_OBJECT_WRITER_H
#define MCLD_ELF_DYNAMIC_SHARED_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ELFWriter.h>
#include <vector>
#include <utility>
#include <mcld/LD/LDContext.h>
#include <mcld/Support/MemoryArea.h>
#include <llvm/ADT/DenseMap.h>
#include <mcld/LD/LDSection.h>
#include <llvm/Support/ELF.h>


namespace mcld
{

class MCLinker;
class TargetLDBackend;

struct SectionExtInfo {
    uint64_t sh_name;
    uint64_t sh_link;
    uint64_t sh_info;
    uint64_t sh_entsize;
};

/** \class ELFDynObjWriter
 *  \brief ELFDynObjWriter writes the dynamic sections.
 */
class ELFDynObjWriter : public DynObjWriter, private ELFWriter
{
public:
  typedef llvm::DenseMap<const LDSection*, SectionExtInfo > SHExtTab_T;

public:
  ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker);
  bool WriteObject();
  ~ELFDynObjWriter();

private:
  const static uint32_t DefaultEABIVersion = 0x05000000;

  uint32_t WriteDynSymTab(LDSection *pSection, uint32_t file_offset);

  uint32_t WriteDynStrTab(LDSection *pSection, uint32_t file_offset);

  uint32_t WriteRelocationTab(LDSection *pSection, uint32_t file_offset,
                              bool addend=false);

  uint32_t WriteDynamicTab(LDSection *pSection, uint32_t file_offset);

  uint32_t WriteSectionData(LDSection *pSection, uint32_t file_offset);

  uint32_t WriteELFHeader(uint32_t sht_offset, uint32_t pht_num);

  uint32_t WriteShStrTab(uint32_t file_offset);

  uint32_t WriteSectionHeader(uint32_t file_offset);

  // ELF::Elf32_Sym
  uint32_t WriteSymbolEntry(uint32_t file_offset,
                            uint32_t name,
                            uint8_t info,
                            uint32_t value,
                            uint32_t size,
                            uint8_t other,
                            uint32_t shndx);

  // ELF::Elf32_Rel
  uint32_t WriteRelEntry(uint32_t file_offset,
                         uint32_t r_offset,
                         uint32_t r_info);

  // ELF::Elf32_Rela
  uint32_t WriteRelaEntry(uint32_t file_offset,
                          uint32_t r_offset,
                          uint32_t r_info,
                          uint32_t r_addend);

  // ELF::Elf32_Shdr
  uint32_t WriteSectionHeaderEntry(LDSection *section, uint32_t file_offset);

  uint32_t WriteProgramHeader(uint32_t file_offset);

  uint32_t WriteProgramHeaderEntry(uint32_t file_offset,
                                   uint32_t p_type,
                                   uint32_t p_flags,
                                   uint64_t p_offset,
                                   uint64_t p_vaddr,
                                   uint64_t p_paddr,
                                   uint64_t p_filesz,
                                   uint64_t p_memsz,
                                   uint64_t p_align);

  SectionExtInfo& getOrCreateSectionExtInfo(const LDSection *section);

  SHExtTab_T shtExtab;

  MCLinker& m_Linker;
  LDContext *m_pContext;
  MemoryArea *m_pMemArea;
};

} // namespace of mcld

#endif

