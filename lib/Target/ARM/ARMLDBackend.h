/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARM_LDBACKEND_H
#define ARM_LDBACKEND_H
#include <mcld/Target/ELFUnixLDBackend.h>

namespace mcld {

//===----------------------------------------------------------------------===//
/// ARMELFLDBackend - linker backend of ARM target of ELF format
///
class ARMELFLDBackend : public ELFUnixLDBackend
{
public:
  ARMELFLDBackend();
  ~ARMELFLDBackend();

private:
  MCELFArchiveTargetReader *createArchiveTargetReader() const;
  MCELFObjectTargetReader *createObjectTargetReader() const;
  MCELFObjectTargetWriter *createObjectTargetWriter() const;

};

//===----------------------------------------------------------------------===//
/// ARMMachOLDBackend - linker backend of ARM target of MachO format
///
/**
class ARMMachOLDBackend : public DarwinARMLDBackend
{
public:
  ARMMachOLDBackend();
  ~ARMMachOLDBackend();

private:
  MCMachOTargetArchiveReader *createTargetArchiveReader() const;
  MCMachOTargetObjectReader *createTargetObjectReader() const;
  MCMachOTargetObjectWriter *createTargetObjectWriter() const;

};
**/
} // namespace of mcld

#endif

