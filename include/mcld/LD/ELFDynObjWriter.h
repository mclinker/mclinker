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
  typedef std::vector< std::pair<const char *, uint32_t> > StrTab;
  typedef llvm::DenseMap<const LDSection*, SectionExtInfo > SHExtTab_T;

public:
  ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker);
  bool WriteObject();
  ~ELFDynObjWriter();

private:
  const static uint32_t DefaultEABIVersion = 0x05000000;

  uint32_t WriteDynSymTab(uint32_t file_offset);

  uint32_t WriteDynStrTab(uint32_t file_offset);

  uint32_t WriteSymbolEntry(uint32_t file_offset,
                        uint32_t name, uint8_t info,
                        uint32_t value, uint32_t size,
                        uint8_t other, uint32_t shndx);

  uint32_t WriteELFHeader(uint32_t file_offset);

  uint32_t WriteShStrTab(uint32_t file_offset);

  uint32_t WriteSectionHeader(uint32_t file_offset);

  uint32_t WriteSectionEnrty(LDSection *section, uint32_t file_offset);

  SectionExtInfo& getOrCreateSectionExtInfo(const LDSection *section);

  // .dynstr
  StrTab dynstrTab;

  // .shstrtab
  StrTab shstrTab;

  SHExtTab_T shtExtab;

  MCLinker& m_Linker;
  LDContext *m_pContext;
  MemoryArea *m_pMemArea;
};

} // namespace of mcld

#endif

