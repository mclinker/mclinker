/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                            *
 ****************************************************************************/
#include "mcld/MC/MCELFObjectReader.h"
#include "mcld/MC/MCLDFile.h"

#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/Triple.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Target/TargetAsmInfo.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetRegistry.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/system_error.h"

using namespace llvm;
using namespace mcld;
using namespace ELF;


static cl::opt<std::string>
MCPU("mcpu",
  cl::desc("Target a specific cpu type (-mcpu=help for details)"),
  cl::value_desc("cpu-name"),
  cl::init(""));


//==========================
// MCELFObjectReader
MCELFObjectReader::MCELFObjectReader(const MCELFObjectTargetReader *pTargetReader)
  : m_pTargetReader(pTargetReader), ShStringTable(0), 
    StringTable(0), DynStringTable(0), SymbolTable(0), 
    DynSymbolTable(0){ }

MCELFObjectReader::~MCELFObjectReader()
{
}

error_code MCELFObjectReader::readObject(const std::string &ObjectFile, 
                                         MCLDFile &LDFile) {
  LDFile.m_pContext = new MCLDContext();

  OwningPtr<MemoryBuffer> File;
  error_code ec;
  ec = MemoryBuffer::getFile(ObjectFile, File);
  FileBase = File->getBufferStart();

  ELFHeader = reinterpret_cast<const Elf32_Ehdr *>(FileBase);
  SectionHeaderTable =
    reinterpret_cast<const Elf32_Shdr *>(FileBase + ELFHeader->e_shoff);

  ShStringTable = getShdrEntry(ELFHeader->e_shstrndx);
  const char *st = reinterpret_cast<const char*>(ShStringTable);


  for (const char *i = reinterpret_cast<const char *>(SectionHeaderTable),
                  *e = i + ELFHeader->e_shnum * ELFHeader->e_shentsize;
                   i != e; i += ELFHeader->e_shentsize) {
    const Elf32_Shdr *sh = reinterpret_cast<const Elf32_Shdr*>(i);
    StringRef SectionName(getNameString(ShStringTable, sh->sh_name));


    if (sh->sh_type == SHT_STRTAB) {
      if (SectionName == ".strtab") {
        if (StringTable != 0)
          continue;
        StringTable = sh;
        const char *StringTableSection = FileBase + sh->sh_offset;
        if (StringTableSection[sh->sh_size - 1] != 0)
          // FIXME: Proper error handling.
          report_fatal_error("String table must end with a null terminator!");
   
        continue;
      }

      else if (SectionName == ".dynstr") {
        if (DynStringTable != 0)
          continue;
        DynStringTable = sh;
      }

      else
        continue;
    }

    if (sh->sh_type == SHT_SYMTAB) {
      if (SymbolTable != 0)
        continue;
      SymbolTable = sh;
      continue;
    }

    if (sh->sh_type == SHT_DYNSYM) {
      if (DynSymbolTable != 0)
        continue;
      DynSymbolTable = sh;
    }

    const MCSectionELF *ShEntry = 
      LDFile.m_pContext->getELFSection(SectionName, sh->sh_type,
                                       0, SectionKind::getReadOnly(),
                                       sh->sh_size, "");
   
    MCSymbol *SymEntry = NULL;
    if (!SectionName.empty()) {
      SymEntry = LDFile.m_pContext->getOrCreateSymbol(SectionName);
      MCSymbolData &SymDataEntry = 
        LDFile.m_pContext->getOrCreateSymbolData(*SymEntry);    
    }
  }


  StringRef SymTabName = getNameString(ShStringTable, SymbolTable->sh_name);

  const MCSectionELF *SymTabSection = 
     LDFile.m_pContext->getELFSection(SymTabName, ELF::SHT_STRTAB, 
                                      0, SectionKind::getReadOnly(),
                                      SymbolTable->sh_size ,"");

  CopySymbolEntryToLDFile(LDFile, SymTabSection);

  return ec;
}

const Elf32_Shdr *MCELFObjectReader::getShdrEntry(Elf32_Half index) const {
  if (index == 0 || index >= ELF::SHN_LORESERVE)
    return 0;

  if (!SectionHeaderTable || index >= ELFHeader->e_shnum)
    // FIXME: Proper error handling.
    report_fatal_error("Invalid section index!");

  return reinterpret_cast<const Elf32_Shdr *>(
         reinterpret_cast<const char *>(SectionHeaderTable)
         + (index * ELFHeader->e_shentsize));
}


const char *MCELFObjectReader::getNameString(Elf32_Half index,
                                         uint32_t offset) const {
  return getNameString(getShdrEntry(index), offset);
}


const char *MCELFObjectReader::getNameString(const Elf32_Shdr *Shdr,
                                         uint32_t offset) const {
  assert(Shdr && Shdr->sh_type == SHT_STRTAB && "Invalid section!");

  if (offset >= Shdr->sh_size)
    // FIXME: Proper error handling.
    report_fatal_error("Symbol name offset outside of string table!");
  return FileBase + Shdr->sh_offset + offset;
}


error_code 
MCELFObjectReader::CopySymbolEntryToLDFile(MCLDFile &File,
                                           const MCSectionELF *SymTabSection) {
  error_code ec;

  for (const char *i = FileBase + SymbolTable->sh_offset,
                  *e = FileBase +
                       SymbolTable->sh_offset +
                       SymbolTable->sh_size;
                  i!=e; i+= SymbolTable->sh_entsize) {
     const Elf32_Sym *SymEntry = reinterpret_cast<const Elf32_Sym *>(i);
     StringRef SymbolName = getNameString(StringTable, SymEntry->st_name);

     MCSectionData &SymTabSD =
       File.m_pContext->getOrCreateSectionData(*SymTabSection);

     MCDataFragment *F = new MCDataFragment(&SymTabSD);

     MCDataFragment *ShndxF = NULL;
     WriteSymbolEntry(F, ShndxF, SymEntry->st_name, SymEntry->st_value,
                      SymEntry->st_size, SymEntry->st_info, 
                      SymEntry->st_other, SymEntry->st_shndx, 1);

     MCSymbol *Sym = NULL; 
     if (!SymbolName.empty()) {
       Sym = File.m_pContext->getOrCreateSymbol(SymbolName);
       MCSymbolData &SymData =File.m_pContext->getOrCreateSymbolData(*Sym);
     }
  }

  return ec;
}


void MCELFObjectReader::WriteSymbolEntry(MCDataFragment *SymtabF,
                                         MCDataFragment *ShndxF,
                                         const uint64_t name,
                                         const uint8_t info, 
                                         const uint64_t value,
                                         const uint64_t size, 
                                         const uint8_t other,
                                         const uint32_t shndx,
                                         bool Reserved) {
  if (ShndxF) {
    if (shndx >= ELF::SHN_LORESERVE && !Reserved)
      String32(*ShndxF, shndx);
  else
    String32(*ShndxF, 0);
  }                      

  uint16_t Index = (shndx >= ELF::SHN_LORESERVE && !Reserved) ?
    uint16_t(ELF::SHN_XINDEX) : shndx;     
           
  if (is64Bit()) {
    String32(*SymtabF, name);  // st_name
    String8(*SymtabF, info);   // st_info
    String8(*SymtabF, other);  // st_other
    String16(*SymtabF, Index); // st_shndx
    String64(*SymtabF, value); // st_value
    String64(*SymtabF, size);  // st_size
  } else {
    String32(*SymtabF, name);  // st_name
    String32(*SymtabF, value); // st_value
    String32(*SymtabF, size);  // st_size
    String8(*SymtabF, info);   // st_info
    String8(*SymtabF, other);  // st_other
    String16(*SymtabF, Index); // st_shndx
  }
}
