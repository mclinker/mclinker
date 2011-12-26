//===- ELFDynObjWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFDynObjWriter.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/ScopedWriter.h>
#include <llvm/Support/ELF.h>
#include <vector>

using namespace llvm;
using namespace mcld;

//==========================
// ELFDynObjWriter
ELFDynObjWriter::ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjWriter(pBackend), ELFWriter(), m_Linker(pLinker) {

  MCLDFile &ldFile = m_Linker.getLDInfo().output();
  m_pMemArea = ldFile.memArea();
  m_pContext = ldFile.context();
}

ELFDynObjWriter::~ELFDynObjWriter()
{
}

bool ELFDynObjWriter::WriteObject()
{
  uint32_t file_offset = target().sectionStartOffset();
  uint32_t pht_num = 0;

  // Assume layout is done correctly.
  for (LDContext::const_sect_iterator I = m_pContext->sectBegin(),
       E = m_pContext->sectEnd();
       I != E;
       ++I) {
       LDSection *pSection = *I;

    switch((*I)->type()) {

      case ELF::SHT_SYMTAB:
        // .symtab could be eliminated.
        break;

      case ELF::SHT_DYNSYM:
        file_offset += WriteDynSymTab(pSection, file_offset);
        break;

      case ELF::SHT_STRTAB:
        file_offset += WriteDynStrTab(pSection, file_offset);
        break;

      case ELF::SHT_REL:
        file_offset += WriteRelocationTab(pSection, file_offset);
        break;

      case ELF::SHT_RELA:
        file_offset += WriteRelocationTab(pSection, file_offset,
                                          /*addend=*/true);
        break;

      case ELF::SHT_DYNAMIC:
        file_offset += WriteDynamicTab(pSection, file_offset);
        break;

      /// other section could use generic section data writer
      case ELF::SHT_NULL:
      case ELF::SHT_HASH:
      case ELF::SHT_NOBITS:
      case ELF::SHT_PROGBITS:
      case ELF::SHT_NOTE:
      case ELF::SHT_SHLIB:
      case ELF::SHT_INIT_ARRAY:
      case ELF::SHT_FINI_ARRAY:
      case ELF::SHT_PREINIT_ARRAY:
      case ELF::SHT_GROUP:
        file_offset += WriteSectionData(pSection, file_offset);
        break;

      case ELF::SHT_SYMTAB_SHNDX:
        llvm::report_fatal_error("SHT_SYMTAB_SHNDX not implmented!");
        break;

      default:
        // Target specific section type
        // Should we call backend to write target-specific section data?
        if ((*I)->type() >= ELF::SHT_LOPROC) {
          file_offset += WriteSectionData(pSection, file_offset);
        }
        // unknown type
        else {
          file_offset += WriteSectionData(pSection, file_offset);
        }
    } // end switch
  }

  // Write out .shstrtab
  file_offset += WriteShStrTab(file_offset);

  // Write out section header table
  WriteSectionHeader(file_offset);

  // Wrtie out program header table
  pht_num = WriteProgramHeader(sizeof(ELF::Elf32_Ehdr));

  // Write out ELF header
  WriteELFHeader(file_offset, pht_num);

  return true;
}

uint32_t ELFDynObjWriter::WriteSectionData(LDSection *pSection, uint32_t file_offset)
{
  // update section header
  pSection->setSize(0);
  pSection->setOffset(0);
  pSection->setAddr(0);

  SectionExtInfo &sec_ext_info = getOrCreateSectionExtInfo(pSection);
  sec_ext_info.sh_link = 0;
  sec_ext_info.sh_info = 0;
  sec_ext_info.sh_entsize = 0;

  // TODO
  return 0;
}

uint32_t ELFDynObjWriter::WriteDynStrTab(LDSection *pSection, uint32_t file_offset)
{
  // FIXME: Due to design change,
  // We should read dynstr from m_Linker.getOrCreateSectData(pSection)

  pSection->setSize(0);
  pSection->setOffset(file_offset);
  // FIXME: addr should be adjust by segment information
  pSection->setAddr(file_offset);

  SectionExtInfo &sec_ext_info = getOrCreateSectionExtInfo(pSection);
  sec_ext_info.sh_link = 0;
  sec_ext_info.sh_info = 0;
  sec_ext_info.sh_entsize = 0;

  return 0;
}

uint32_t ELFDynObjWriter::WriteDynSymTab(LDSection *pSection, uint32_t file_offset)
{
  // FIXME: Due to design change,
  // We should read dynsym from m_Linker.getOrCreateSectData(pSection)

  LDContext *context = m_Linker.getLDInfo().output().context();
  uint32_t dynsym_cnt = 1;
  uint32_t section_offset = file_offset;

  // The first entry is undefined.
  file_offset += WriteSymbolEntry(file_offset, 0, 0, 0, 0, 0, ELF::SHN_UNDEF);

  for (LDContext::sym_iterator I = context->symBegin<LDContext::RegSymTable>(),
       E = context->symEnd<LDContext::RegSymTable>();
       I != E;
       ++I) {

    if (!(*I)->isDyn())
      continue;

    uint8_t resolve_binding = (*I)->binding();
    uint8_t elf_binding;

    switch (resolve_binding)
    {
      case ResolveInfo::Absolute:
      case ResolveInfo::Global: elf_binding = ELF::STB_GLOBAL; break;
      case ResolveInfo::Local: elf_binding = ELF::STB_LOCAL; break;
      case ResolveInfo::Weak: elf_binding = ELF::STB_WEAK; break;
      case ResolveInfo::NoneBinding:
      default: assert(0 && "unknown binding!");
    }

    uint8_t info = (elf_binding << 4) + (((*I)->type()) & 0x0f);


    file_offset += WriteSymbolEntry(file_offset,
                                    0,       /* name, TODO  */
                                    (*I)->value(), /* value */
                                    (*I)->size(),  /* size  */
                                    info,          /* info  */
                                    (*I)->other(), /* other */
                                    0        /* shndx, TODO */);
    ++dynsym_cnt;
  }

  // update .dynsym section header
  pSection->setSize(dynsym_cnt*sizeof(ELF::Elf32_Sym));
  pSection->setOffset(section_offset);

  pSection->setAddr(section_offset);

  SectionExtInfo &sec_ext_info = getOrCreateSectionExtInfo(pSection);
  sec_ext_info.sh_link = 0; // TODO: get .dynstr index from Layout
  sec_ext_info.sh_info = 0;
  sec_ext_info.sh_entsize = sizeof(ELF::Elf32_Sym);
  // FIXME: addr should be adjust by segment information
  return dynsym_cnt*sizeof(ELF::Elf32_Sym);
}

uint32_t ELFDynObjWriter::WriteDynamicTab(LDSection *pSection, uint32_t file_offset)
{
  // TODO
  return 0;
}

uint32_t ELFDynObjWriter::WriteRelocationTab(LDSection *pSection,
                                             uint32_t file_offset,
                                             bool addend)
{
  // TODO
  return 0;
}

uint32_t ELFDynObjWriter::WriteRelEntry(uint32_t file_offset,
                                        uint32_t r_offset,
                                        uint32_t r_info)
{
  MemoryRegion* pRegion = m_pMemArea->request(file_offset,
                                  sizeof(ELF::Elf32_Rel));
  ScopedWriter *pWriter = new ScopedWriter(pRegion, true);

  pWriter->Write32(r_offset);
  pWriter->Write32(r_info);

  delete pWriter;
  return sizeof(ELF::Elf32_Rel);
}

uint32_t ELFDynObjWriter::WriteRelaEntry(uint32_t file_offset,
                                        uint32_t r_offset,
                                        uint32_t r_info,
                                        uint32_t r_addend)
{
  MemoryRegion* pRegion = m_pMemArea->request(file_offset,
                                  sizeof(ELF::Elf32_Rel));
  ScopedWriter *pWriter = new ScopedWriter(pRegion, true);

  pWriter->Write32(r_offset);
  pWriter->Write32(r_info);
  pWriter->Write32(r_addend);

  delete pWriter;
  return sizeof(ELF::Elf32_Rela);
}


uint32_t ELFDynObjWriter::WriteSymbolEntry(uint32_t file_offset,
                                           uint32_t name,
                                           uint8_t info,
                                           uint32_t value,
                                           uint32_t size,
                                           uint8_t other,
                                           uint32_t shndx)
{
  MemoryRegion* pRegion = m_pMemArea->request(file_offset,
                                  sizeof(ELF::Elf32_Sym));
  ScopedWriter *pWriter = new ScopedWriter(pRegion, true);

  pWriter->Write32(name);  // st_name
  pWriter->Write32(value); // st_value
  pWriter->Write32(size);  // st_size
  pWriter->Write8(info);   // st_info
  pWriter->Write8(other);  // st_other
  pWriter->Write16(shndx); // st_shndx

  delete pWriter;
  return sizeof(ELF::Elf32_Sym);
}

uint32_t ELFDynObjWriter::WriteELFHeader(uint32_t sht_offset, uint32_t pht_num)
{
    //Currently only support 32bit file type
    bool is32BitClass = (32 == target().bitclass());
    ELF::Elf32_Ehdr elf_header;

    memcpy(elf_header.e_ident, ELF::ElfMagic, strlen(ELF::ElfMagic));
    elf_header.e_ident[ELF::EI_CLASS] = (is32BitClass) ? ELF::ELFCLASS32 : ELF::ELFCLASS64;
    elf_header.e_ident[ELF::EI_DATA] = target().isLittleEndian() ?
                                                         ELF::ELFDATA2LSB : ELF::ELFDATA2MSB;
    elf_header.e_ident[ELF::EI_VERSION] = ELF::EV_CURRENT;
    elf_header.e_ident[ELF::EI_OSABI] = 0x0;
    elf_header.e_ident[ELF::EI_ABIVERSION] = 0x0;

    elf_header.e_type = ELF::ET_DYN;
    elf_header.e_machine = target().machine();
    elf_header.e_version = elf_header.e_ident[ELF::EI_VERSION];
    // FIXME:
    elf_header.e_entry   = 0x0;
    elf_header.e_phoff   = sizeof(ELF::Elf32_Ehdr);
    elf_header.e_shoff   = sht_offset;

    switch (target().machine())
    {
        // FIXME:
        case ELF::EM_ARM :
            elf_header.e_flags = ELF::EF_ARM_EABIMASK & DefaultEABIVersion;
            break;
        case ELF::EM_386 :
        case ELF::EM_MIPS :
        default :
            elf_header.e_flags = 0x0;
            break;
    }

    elf_header.e_ehsize = sizeof(ELF::Elf32_Ehdr);
    elf_header.e_phentsize = sizeof(ELF::Elf32_Phdr);
    // FIXME:
    elf_header.e_phnum = pht_num;
    elf_header.e_shentsize = sizeof(ELF::Elf32_Shdr);
    elf_header.e_shnum = m_pContext->numOfSections();
    elf_header.e_shstrndx = m_pContext->numOfSections() - 1;

    MemoryRegion *mr = m_pMemArea->request(0, sizeof(ELF::Elf32_Ehdr));
    memcpy(mr->start(), &elf_header, sizeof(ELF::Elf32_Ehdr));
    mr->sync();

    return elf_header.e_ehsize;
}

uint32_t ELFDynObjWriter::WriteShStrTab(uint32_t file_offset)
{
    // Create .shstrtab section
    LDSection *m_pShStrTab = new LDSection(".shstrtab",
                                          LDFileFormat::StringTable,
                                          ELF::SHT_STRTAB,
                                          ELF::SHF_STRINGS);
    m_pContext->getSectionTable().push_back(m_pShStrTab);

    // Write out section names and fill in sh_name index
    uint32_t sh_name_idx = 0;

    // Iterate all the the sections in SectionTable
    LDContext::const_sect_iterator sect_iter;
    for (sect_iter = m_pContext->sectBegin(); sect_iter < m_pContext->sectEnd(); ++sect_iter)
    {
        const LDSection &section = *(*sect_iter);
        SectionExtInfo & sec_ext_info = getOrCreateSectionExtInfo(&section);
        sec_ext_info.sh_name = sh_name_idx;

        MemoryRegion *mr = m_pMemArea->request((file_offset + sh_name_idx),
                                                section.name().size() + 1 );
        memcpy(mr->start(), section.name().data(), section.name().size() + 1);
        mr->sync();

        sh_name_idx += section.name().size() + 1;
    }

    m_pShStrTab->setOffset(file_offset);
    m_pShStrTab->setSize(sh_name_idx);

    return sh_name_idx;
}

uint32_t ELFDynObjWriter::WriteSectionHeader(uint32_t file_offset)
{
    uint32_t sec_off = 0;

    // Iterate the SectionTable in LDContext
    LDContext::const_sect_iterator sect_iter;
    for (sect_iter = m_pContext->sectBegin(); sect_iter < m_pContext->sectEnd(); ++sect_iter)
    {
        LDSection *section = *sect_iter;
        sec_off += WriteSectionHeaderEntry(section, file_offset + sec_off);
    }

    return sec_off;
}

uint32_t ELFDynObjWriter::WriteSectionHeaderEntry(LDSection *section, uint32_t file_offset)
{
    SectionExtInfo & sec_ext_info = getOrCreateSectionExtInfo(section);

    ELF::Elf32_Shdr sec_header;
    sec_header.sh_name      = sec_ext_info.sh_name;
    sec_header.sh_type      = section->type();
    sec_header.sh_flags     = section->flag();
    sec_header.sh_addr      = section->addr();
    sec_header.sh_offset    = section->offset();
    sec_header.sh_size      = section->size();
    sec_header.sh_link      = sec_ext_info.sh_link;
    sec_header.sh_info      = sec_ext_info.sh_info;

    if (NULL != section->getSectionData())
        sec_header.sh_addralign = section->getSectionData()->getAlignment();
    else
        sec_header.sh_addralign = 0x0;

    sec_header.sh_entsize   = sec_ext_info.sh_entsize;

    MemoryRegion *mr = m_pMemArea->request(file_offset, sizeof(ELF::Elf32_Shdr));
    memcpy(mr->start(), &sec_header, sizeof(ELF::Elf32_Shdr));
    mr->sync();

    return  sizeof(ELF::Elf32_Shdr);
}

uint32_t ELFDynObjWriter::WriteProgramHeader(uint32_t file_offset)
{
    uint32_t prog_off = 0;
    uint32_t prg_header_num = 0;
    LDSection *section;

    //Segments before text segment
    switch (target().machine())
    {
        case ELF::EM_ARM :
            section = m_pContext->getSection(".ARM.exidx");
            if (NULL != section)
            {
                prog_off += WriteProgramHeaderEntry(file_offset + prog_off,
                                                    0x70000001, //PT_ARM_EXIDX
                                                    ELF::PF_R,
                                                    section->offset(),
                                                    section->addr(),
                                                    section->addr(),
                                                    section->size(),
                                                    section->size(),
                                                    (NULL == section->getSectionData()) ?
                                                     0 : section->getSectionData()->getAlignment());
                ++prg_header_num;
            }
            break;
        default :
            break;
    }

    //text segment

    //data segment

    //dynamic segment

    //note segment

    //segment after above

    return prg_header_num;

}

uint32_t ELFDynObjWriter::WriteProgramHeaderEntry(uint32_t file_offset,
                                                  uint32_t p_type,
                                                  uint32_t p_flags,
                                                  uint64_t p_offset,
                                                  uint64_t p_vaddr,
                                                  uint64_t p_paddr,
                                                  uint64_t p_filesz,
                                                  uint64_t p_memsz,
                                                  uint64_t p_align)
{
    // 32bit Program header entry
    ELF::Elf32_Phdr phdr;
    phdr.p_type   = p_type;
    phdr.p_offset = p_offset;
    phdr.p_vaddr  = p_vaddr;
    phdr.p_paddr  = p_paddr;
    phdr.p_filesz = p_filesz;
    phdr.p_memsz  = p_memsz;
    phdr.p_flags  = p_flags;
    phdr.p_align  = p_align;

    MemoryRegion *mr = m_pMemArea->request(file_offset, sizeof(ELF::Elf32_Phdr));
    memcpy(mr->start(), &phdr, sizeof(ELF::Elf32_Phdr));
    mr->sync();

    return sizeof(ELF::Elf32_Phdr);
}

SectionExtInfo& ELFDynObjWriter::getOrCreateSectionExtInfo(const LDSection *section)
{
    SHExtTab_T::iterator iter;
    if (shtExtab.count(section))
    {
        iter = shtExtab.find(section);
    }
    else
    {
        SectionExtInfo *sec_ext_info = new SectionExtInfo();
        iter = shtExtab.insert( std::pair<const LDSection*,
                                SectionExtInfo>(section, *sec_ext_info)).first;
    }
    return (*iter).second;
}

