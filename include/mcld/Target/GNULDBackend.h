//===- GNULDBackend.h -----------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef GNU_LDBACKEND_H
#define GNU_LDBACKEND_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include "mcld/Target/TargetLDBackend.h"

namespace mcld
{

class MCObjectReader;
class MCELFObjectTargetReader;
class MCArchiveReader;
class MCGNUArchiveReader;
class LDWriter;
class ELFDSOWriter;
class ELFEXEWriter;
class MemoryArea;

/// class GNULDBackend:
/// brief GNULDBackend provides a common interface for all GNU Unix-OS
/// LDBackend.
class GNULDBackend : public TargetLDBackend
{
protected:
  GNULDBackend();
public:
  virtual ~GNULDBackend();

  MCArchiveReader *getArchiveReader();
  MCObjectReader  *getObjectReader();
  LDWriter *getDSOWriter(MemoryArea *Area, bool _IsLittleEndian);
  LDWriter *getEXEWriter(MemoryArea *Area, bool _IsLittleEndian);

protected:
  virtual MCELFObjectTargetReader *createObjectTargetReader() const = 0;

  MCObjectReader *m_pObjectReader;
  MCArchiveReader *m_pArchiveReader;
  ELFDSOWriter *m_pELFDSOWriter;
  ELFEXEWriter *m_pELFEXEWriter;
};

} // namespace of mcld

#endif

