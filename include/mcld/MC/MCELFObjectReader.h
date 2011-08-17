/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                            *
 ****************************************************************************/
#ifndef MCELFOBJECTREADER_H
#define MCELFOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "llvm/MC/MCObjectWriter.h"
#include "llvm/Support/ELF.h"

#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCObjectReader.h"

using namespace llvm;
using namespace ELF;

namespace mcld
{
class MCELFObjectTargetReader;

//MCELFObjectReader reads target-independent parts of ELF object file
class MCELFObjectReader : public MCObjectReader
{
public:
  MCELFObjectReader(const MCELFObjectTargetReader *pTargetReader);
  ~MCELFObjectReader();

  bool isMyFormat(MCLDFile &File) const;
  Input::Type fileType(MCLDFile &File) const;
  virtual llvm::error_code readObject(const std::string &ObjectFile,
                                      MCLDFile &File);

  //FIXME:these should be implemented by MCObjectTargetReader
  bool isLittleEndian() const { return true; }
  bool is64Bit() const { return false; }

private:
  const MCELFObjectTargetReader *m_pTargetReader;

  const char *FileBase;
  const Elf32_Ehdr *ELFHeader;
  const Elf32_Shdr *SectionHeaderTable;

  const Elf32_Shdr *StringTable;
  const Elf32_Shdr *ShStringTable;
  const Elf32_Shdr *DynStringTable;

  const Elf32_Shdr *SymbolTable;
  const Elf32_Shdr *DynSymbolTable;

  const Elf32_Shdr *getShdrEntry(Elf32_Half index) const;
  const char *getNameString(Elf32_Half index, uint32_t offset) const;
  const char *getNameString(const Elf32_Shdr *section, uint32_t offset) const;

  error_code CopySymbolEntryToLDFile(MCLDFile &File, const MCSectionELF *SymTabSection);

  void WriteSymbolEntry(MCDataFragment *SymtabF, 
                        MCDataFragment *ShndxF,
                        uint64_t name,
                        uint8_t info, uint64_t value,
                        uint64_t size, uint8_t other,
                        uint32_t shndx,
                        bool Reserved);
 
  //String Writing
  void StringLE16(char *buf, uint16_t Value) {
    buf[0] = char(Value >> 0);
    buf[1] = char(Value >> 8);
  }

  void StringLE32(char *buf, uint32_t Value) {
    StringLE16(buf, uint16_t(Value >> 0));
    StringLE16(buf + 2, uint16_t(Value >> 16));
  }

  void StringLE64(char *buf, uint64_t Value) {
    StringLE32(buf, uint32_t(Value >> 0));
    StringLE32(buf + 4, uint32_t(Value >> 32));
  }

  void StringBE16(char *buf ,uint16_t Value) {
    buf[0] = char(Value >> 8);
    buf[1] = char(Value >> 0);
  }

  void StringBE32(char *buf, uint32_t Value) {
    StringBE16(buf, uint16_t(Value >> 16));
    StringBE16(buf + 2, uint16_t(Value >> 0));
  }

  void StringBE64(char *buf, uint64_t Value) {
    StringBE32(buf, uint32_t(Value >> 32));
    StringBE32(buf + 4, uint32_t(Value >> 0));
  }

  void String8(MCDataFragment &F, uint8_t Value) {
    char buf[1];
    buf[0] = Value;
    F.getContents() += StringRef(buf, 1);
  }

  void String16(MCDataFragment &F, uint16_t Value) {
    char buf[2];
    if (isLittleEndian())
      StringLE16(buf,Value);
    else 
      StringBE16(buf, Value);
    F.getContents() +=StringRef(buf, 2);
  }

  void String32(MCDataFragment &F, uint32_t Value) {
    char buf[4];
    if (isLittleEndian())
      StringLE32(buf, Value);
    else
      StringBE32(buf, Value);
    F.getContents() += StringRef(buf, 4);
  }

  void String64(MCDataFragment &F, uint64_t Value) {
    char buf[8];
    if (isLittleEndian())
      StringLE64(buf, Value);
    else
      StringBE64(buf, Value);
    F.getContents() += StringRef(buf, 8);
  }

};

} // namespace of mcld

#endif
