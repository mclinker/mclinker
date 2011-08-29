/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef X86_LDBACKEND_H
#define X86_LDBACKEND_H
#include <mcld/Target/GNULDBackend.h>

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

