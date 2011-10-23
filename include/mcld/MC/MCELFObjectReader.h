//===- MCELFObjectReader.h ------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_OBJECT_READER_H
#define MCLD_ELF_OBJECT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/MC/MCObjectReader.h"
#include "llvm/Support/ELF.h"

namespace llvm {
class MCDataFragment;
}

namespace mcld
{
class Input;
class MCELFObjectTargetReader;

//MCELFObjectReader reads target-independent parts of ELF object file
class MCELFObjectReader : public MCObjectReader
{
public:
  MCELFObjectReader(MCELFObjectTargetReader *pTargetReader);
  ~MCELFObjectReader();

  Input::Type fileType(Input &pFile) const;
  llvm::error_code readObject(const std::string &ObjectFile,
                                      MCLDFile &File);

  bool isMyFormat(Input &pFile) const;

  //FIXME:these should be implemented by MCObjectTargetReader
  bool isLittleEndian() const { return true; }
  bool is64Bit() const { return false; }

  bool hasRelocationAddend();
  unsigned getRelocType(const MCValue& Target,
                        const MCFixup& Fixup,
                        bool IsPCRel,
                        bool IsRelocWithSymbol,
                        int64_t Addend);

private:
  MCELFObjectTargetReader *m_pTargetReader;

  const char *FileBase;
  const llvm::ELF::Elf32_Ehdr *ELFHeader;
  const llvm::ELF::Elf32_Shdr *SectionHeaderTable;

  const llvm::ELF::Elf32_Shdr *StringTable;
  const llvm::ELF::Elf32_Shdr *ShStringTable;
  const llvm::ELF::Elf32_Shdr *DynStringTable;

  const llvm::ELF::Elf32_Shdr *SymbolTable;
  const llvm::ELF::Elf32_Shdr *DynSymbolTable;

  const llvm::ELF::Elf32_Shdr *getShdrEntry(llvm::ELF::Elf32_Half index) const;
  const char *getNameString(llvm::ELF::Elf32_Half index, uint32_t offset) const;
  const char *getNameString(const llvm::ELF::Elf32_Shdr *section, uint32_t offset) const;

  llvm::error_code CopySymbolEntryToLDFile(MCLDFile &File,
                                     const llvm::ELF::Elf32_Shdr *ShSym,
                                     const llvm::ELF::Elf32_Shdr *ShStr);

  void WriteSymbolEntry(llvm::MCDataFragment *SymtabF,
                        llvm::MCDataFragment *ShndxF,
                        uint64_t name,
                        uint8_t info, uint64_t value,
                        uint64_t size, uint8_t other,
                        uint32_t shndx,
                        bool Reserved);
};

} // namespace of mcld

#endif
