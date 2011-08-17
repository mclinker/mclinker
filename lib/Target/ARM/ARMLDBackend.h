/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ARM_LDBACKEND_H
#define ARM_LDBACKEND_H
#include <mcld/Target/GNULDBackend.h>

namespace mcld {

//===----------------------------------------------------------------------===//
/// ARMGNULDBackend - linker backend of ARM target of GNU ELF format
///
class ARMGNULDBackend : public GNULDBackend
{
public:
  ARMGNULDBackend();
  ~ARMGNULDBackend();

  // TODO(Nowar): Add correct behavior
  virtual void readRelocation() {}

private:
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

