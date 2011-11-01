//===- MCELFObjectReader.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/MC/MCELFObjectReader.h"
#include "mcld/MC/MCELFObjectTargetReader.h"
#include "mcld/MC/MCLDFile.h"

#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/Triple.h"
#include "llvm/CodeGen/MachineModuleInfo.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Target/TargetLowering.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ELF.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/system_error.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <string>

using namespace llvm;
using namespace mcld;
using namespace ELF;

//==========================
// MCELFObjectReader
MCELFObjectReader::MCELFObjectReader(MCELFObjectTargetReader *pTargetReader)
  : m_pTargetReader(pTargetReader), ShStringTable(0),
    StringTable(0), DynStringTable(0), SymbolTable(0),
    DynSymbolTable(0) {}

MCELFObjectReader::~MCELFObjectReader()
{
}

bool MCELFObjectReader::isMyFormat(mcld::Input &pFile) const
{
  if( !exists(pFile.path()) || is_directory(pFile.path()))
    return false;

  int fd = 0;
  if(-1 == (fd=open(pFile.path().native().c_str(), 0644)))
    llvm::report_fatal_error(std::string("can not open: ")+pFile.path().native());

  unsigned char magic[16];
  lseek(fd, 0, SEEK_SET);
  if (read(fd, &magic, sizeof(magic)))
    close(fd);
  else
    return false;

  return (magic[0] == 0x7F && magic[1] == 'E' &&
      magic[2] == 'L' && magic[3] == 'F');
}

Input::Type MCELFObjectReader::fileType(mcld::Input &pFile) const
{
  int fd;
  uint16_t e_type;
  Input::Type result;

  fd = open(pFile.path().c_str(), 0644);
  lseek(fd, 0, SEEK_SET);
  lseek(fd, sizeof(char)*16, SEEK_SET);
  if (read(fd, &e_type, sizeof(e_type))) {
    switch(e_type) {
    case ET_REL:
      result = Input::Object;
      break;
    case ET_DYN:
      result = Input::DynObj;
      break;
    default:
      result = Input::Unknown;
    }
    close(fd);
  }
  else {
    llvm::report_fatal_error(std::string("can not read: ")+pFile.path().native());
  }

  return result;
}

error_code MCELFObjectReader::readObject(const std::string &ObjectFile,
                                         MCLDFile &LDFile)
{
  error_code ec;
  return ec;
}

// FIXME: Should it be removed?
bool MCELFObjectReader::hasRelocationAddend() {
  return false;
}


// FIXME: Should it be removed?
unsigned MCELFObjectReader::getRelocType(const MCValue& Target,
                                         const MCFixup& Fixup,
                                         bool IsPCRel,
                                         bool IsRelocWithSymbol,
                                         int64_t Addend) {
  return 0;
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
                                           const Elf32_Shdr *ShSym,
                                           const Elf32_Shdr *ShStr) {
  error_code ec;

  return ec;
}


void MCELFObjectReader::WriteSymbolEntry(llvm::MCDataFragment *SymtabF,
                                         llvm::MCDataFragment *ShndxF,
                                         const uint64_t name,
                                         const uint8_t info,
                                         const uint64_t value,
                                         const uint64_t size,
                                         const uint8_t other,
                                         const uint32_t shndx,
                                         bool Reserved) {
}
