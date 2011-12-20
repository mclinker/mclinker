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

using namespace llvm;
using namespace mcld;

//==========================
// ELFDynObjWriter
ELFDynObjWriter::ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjWriter(pBackend), ELFWriter(), m_Linker(pLinker) {

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

  if (!WriteELFHeader())
    return false;

  return true;
}

uint32_t ELFDynObjWriter::WriteDynStrTab(uint32_t file_offset)
{
  assert(!dynstrTab.empty());

  uint32_t str_size = dynstrTab.back().second;
  dynstrTab.pop_back();

  MemoryArea *mem = m_Linker.getLDInfo().output().memArea();
  MemoryRegion* pRegion = mem->request(file_offset, str_size);
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

  delete pWriter;
  return str_size;
}

uint32_t ELFDynObjWriter::WriteDynSymTab(uint32_t file_offset)
{
  LDContext *context = m_Linker.getLDInfo().output().context();
  uint32_t str_index = 1;
  uint32_t dynsym_cnt = 1;

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

  return dynsym_cnt * sizeof(ELF::Elf32_Sym);
}

uint32_t ELFDynObjWriter::WriteSymbolEntry(uint32_t file_offset,
                                       uint32_t name, uint8_t info,
                                       uint32_t value, uint32_t size,
                                       uint8_t other, uint32_t shndx)
{
  MemoryArea *mem = m_Linker.getLDInfo().output().memArea();
  MemoryRegion* pRegion = mem->request(file_offset, sizeof(ELF::Elf32_Sym));
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

bool ELFDynObjWriter::WriteELFHeader()
{
  MCLDInfo &ldInfo = m_Linker.getLDInfo();
  MCLDFile &ldFile = ldInfo.output();
  MemoryArea *memArea = ldFile.memArea();

  bool is32BitClass = (32 == target().bitclass());

  ScopedWriter *pWriter = new ScopedWriter(memArea->request(0,
    (is32BitClass) ? sizeof(ELF::Elf32_Ehdr) : sizeof(ELF::Elf64_Ehdr)), true);

  /* ELF Identification */
  pWriter->Write8(ELF::ElfMagic[0]);   //EI_MAG0
  pWriter->Write8(ELF::ElfMagic[1]);   //EI_MAG1
  pWriter->Write8(ELF::ElfMagic[2]);   //EI_MAG2
  pWriter->Write8(ELF::ElfMagic[3]);   //EI_MAG3
  pWriter->Write8((is32BitClass) ? ELF::ELFCLASS32 : ELF::ELFCLASS64);     //EI_CLASS
  pWriter->Write8(target().isLittleEndian() ? ELF::ELFDATA2LSB : ELF::ELFDATA2MSB); //EI_DATA
  pWriter->Write8(ELF::EV_CURRENT);    //EI_VERSION
  pWriter->Write8(0x0);                //EI_OSABI
  pWriter->Write8(0x0);                //EI_ABIVERSION
  for (int i = (ELF::EI_NIDENT - ELF::EI_PAD); i > 0; --i)
    pWriter->Write8(0x0);              //EI_PAD

  pWriter->Write16(ELF::ET_DYN);            //e_type
  pWriter->Write16(target().machine());     //e_machine
  pWriter->Write32(ELF::EV_CURRENT);        //e_version

  //FIXME:
  if (is32BitClass)
  {
    pWriter->Write32(0x0);                  //e_entry, it seems pointing to the first .text section
    pWriter->Write32(0x0);                  //e_phoff = program header offset
    pWriter->Write32(0x0);                  //e_shoff = section header offset FIXME
  }else{
    pWriter->Write64(0x0);
    pWriter->Write64(0x0);
    pWriter->Write64(0x0);
  }

  //FIXME: ARM should add enrty point judgement
  if (ELF::EM_ARM == target().machine())    //e_flags
    pWriter->Write32(ELF::EF_ARM_EABIMASK & DefaultEABIVersion);
  else
    pWriter->Write32(0x0);
  pWriter->Write16((is32BitClass) ?
                    sizeof(ELF::Elf32_Ehdr) : sizeof(ELF::Elf64_Ehdr)); //e_ehsize = elf header size

  //FIXME
  pWriter->Write16(0x0);                    //e_phentsize
  pWriter->Write16(0x0);                    //e_phnum

  //FIXME
  pWriter->Write16((is32BitClass) ?
                    sizeof(ELF::Elf32_Shdr) : sizeof(ELF::Elf64_Shdr)); //e_shentsize
  pWriter->Write16(0x0);                    //e_shnum
  pWriter->Write16(ELF::SHN_UNDEF);         //e_shstrndx

  delete pWriter;

  return true;
}
