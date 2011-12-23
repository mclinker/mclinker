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
  uint32_t file_offset = (32 == target().bitclass()) ?
                       sizeof(ELF::Elf32_Ehdr) : sizeof(ELF::Elf32_Ehdr);

  // Write out .dynsym
  file_offset += WriteDynSymTab(file_offset);

  // Write out .dynstr
  file_offset += WriteDynStrTab(file_offset);

  // Write out .shstrtab
  file_offset += WriteShStrTab(file_offset);

  // Write out section header table
  WriteSectionHeader(file_offset);

  // Write out ELF header
  WriteELFHeader(file_offset);

  return true;
}

uint32_t ELFDynObjWriter::WriteDynStrTab(uint32_t file_offset)
{
  assert(!dynstrTab.empty());

  uint32_t str_size = dynstrTab.back().second;
  dynstrTab.pop_back();
  uint32_t section_offset = file_offset;

  MemoryRegion* pRegion = m_pMemArea->request(file_offset, str_size);
  ScopedWriter *pWriter = new ScopedWriter(pRegion, true);

  for (StrTab::iterator I = dynstrTab.begin(), E = dynstrTab.end();
       I != E;
       ++I) {
    const char *c = I->first;
    while (*c != '\0') {
      pWriter->Write8(*c);
      c++;
    }
    pWriter->Write8(0);
  }

  // update .dynsym section header
  LDSection *dynstr = m_pContext->getSection(".dynstr");
  dynstr->setSize(str_size);
  dynstr->setOffset(section_offset);
  dynstr->setAddr(section_offset);

  SectionExtInfo &sec_ext_info = getOrCreateSectionExtInfo(dynstr);
  sec_ext_info.sh_link = 0;
  sec_ext_info.sh_info = 0;
  sec_ext_info.sh_entsize = 0;

  delete pWriter;
  return str_size;
}

uint32_t ELFDynObjWriter::WriteDynSymTab(uint32_t file_offset)
{
  LDContext *context = m_Linker.getLDInfo().output().context();
  uint32_t str_index = 1;
  uint32_t dynsym_cnt = 1;
  uint32_t section_offset = file_offset;

  // The first entry is undefined.
  file_offset += WriteSymbolEntry(file_offset, 0, 0, 0, 0, 0, ELF::SHN_UNDEF);
  dynstrTab.push_back(std::make_pair("",0));

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
                                    str_index,     /* name  */
                                    (*I)->value(), /* value */
                                    (*I)->size(),  /* size  */
                                    info,          /* info  */
                                    (*I)->other(), /* other */
                                    0        /* shndx, TODO */);
    // construct dynamic string
    dynstrTab.push_back(std::make_pair((*I)->name(), str_index));
    str_index += (*I)->nameSize() + 1;
    ++dynsym_cnt;
  }

  // save the total string size at the end
  dynstrTab.push_back(std::make_pair("",str_index));

  // update .dynsym section header
  LDSection *dynsym = m_pContext->getSection(".dynsym");
  dynsym->setSize(dynsym_cnt * sizeof(ELF::Elf32_Sym));
  dynsym->setOffset(section_offset);
  dynsym->setAddr(section_offset);

  SectionExtInfo &sec_ext_info = getOrCreateSectionExtInfo(dynsym);
  sec_ext_info.sh_link = 0; // TODO: get .dynstr index from Layout
  sec_ext_info.sh_info = 0;
  sec_ext_info.sh_entsize = sizeof(ELF::Elf32_Sym);

  return dynsym_cnt * sizeof(ELF::Elf32_Sym);
}

uint32_t ELFDynObjWriter::WriteSymbolEntry(uint32_t file_offset,
                                       uint32_t name, uint8_t info,
                                       uint32_t value, uint32_t size,
                                       uint8_t other, uint32_t shndx)
{
  MemoryRegion* pRegion = m_pMemArea->request(file_offset, sizeof(ELF::Elf32_Sym));
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

uint32_t ELFDynObjWriter::WriteELFHeader(uint32_t file_offset)
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
    elf_header.e_shoff   = file_offset;

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
    elf_header.e_phnum = 0x0;
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
        sec_off += WriteSectionEnrty(section, file_offset + sec_off);
    }

    return sec_off;
}

uint32_t ELFDynObjWriter::WriteSectionEnrty(LDSection *section, uint32_t file_offset)
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
    //Currently we still couldn't get MCSectionData
    //sec_header.sh_addralign = m_Linker.getOrCreateSectData(section)->getAlignment();
    sec_header.sh_addralign = 0;
    sec_header.sh_entsize   = sec_ext_info.sh_entsize;

    MemoryRegion *mr = m_pMemArea->request(file_offset, sizeof(ELF::Elf32_Shdr));
    memcpy(mr->start(), &sec_header, sizeof(ELF::Elf32_Shdr));
    mr->sync();

    return  sizeof(ELF::Elf32_Shdr);
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

