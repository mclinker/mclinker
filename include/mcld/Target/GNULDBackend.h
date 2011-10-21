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

class MCArchiveReader;
class MCObjectReader;
class MCObjectWriter;
class MCGNUArchiveReader;
class MCELFObjectTargetReader;
class MCELFObjectTargetWriter;

/** \class GNULDBackend
 *  \brief GNULDBackend provides a common interface for all GNU Unix-OS
 *  LDBackend.
 */
class GNULDBackend : public TargetLDBackend
{
protected:
  GNULDBackend();
public:
  virtual ~GNULDBackend();

  MCArchiveReader *getArchiveReader();
  MCObjectReader  *getObjectReader();
  MCObjectWriter  *getObjectWriter();

protected:
  virtual MCELFObjectTargetReader *createObjectTargetReader() const = 0;
  virtual MCELFObjectTargetWriter *createObjectTargetWriter() const = 0;

  MCObjectReader *m_pObjectReader;
  MCArchiveReader *m_pArchiveReader;
  MCObjectWriter *m_pObjectWriter;
};

} // namespace of mcld

#endif

