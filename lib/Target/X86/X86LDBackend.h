//===- X86LDBackend.h -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_LDBACKEND_H
#define X86_LDBACKEND_H
#include "mcld/Target/GNULDBackend.h"

namespace mcld {

//===----------------------------------------------------------------------===//
/// X86GNULDBackend - linker backend of X86 target of GNU ELF format
///
class X86GNULDBackend : public GNULDBackend
{
public:
  X86GNULDBackend();
  ~X86GNULDBackend();

private:
  MCELFObjectTargetReader *createObjectTargetReader() const;
  MCELFObjectTargetWriter *createObjectTargetWriter() const;
};

//===----------------------------------------------------------------------===//
/// X86MachOLDBackend - linker backend of X86 target of MachO format
///
/**
class X86MachOLDBackend : public DarwinX86LDBackend
{
public:
  X86MachOLDBackend();
  ~X86MachOLDBackend();

private:
  MCMachOTargetArchiveReader *createTargetArchiveReader() const;
  MCMachOTargetObjectReader *createTargetObjectReader() const;
  MCMachOTargetObjectWriter *createTargetObjectWriter() const;

};
**/
} // namespace of mcld

#endif

