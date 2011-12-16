//===- ARMLDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef ARM_LDBACKEND_H
#define ARM_LDBACKEND_H

#include "ARMGOT.h"
#include "mcld/Target/GNULDBackend.h"

namespace mcld {

//===----------------------------------------------------------------------===//
/// ARMGNULDBackend - linker backend of ARM target of GNU ELF format
///
class ARMGNULDBackend : public GNULDBackend
{
public:
  ARMGNULDBackend();
  ~ARMGNULDBackend();

  /// getRelocFactory
  RelocationFactory* getRelocFactory();

  uint32_t machine() const;

  bool isLittleEndian() const;

  unsigned int bitclass() const
  { return 32; }

  ARMGOT& getGOT()
  { return m_GOT; }

  const ARMGOT& getGOT() const
  { return m_GOT; }

private:
  RelocationFactory* m_pRelocFactory;
  ARMGOT m_GOT;
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

