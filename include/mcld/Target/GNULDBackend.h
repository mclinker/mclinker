//===- GNULDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
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

#include "mcld/LD/GNUArchiveReader.h"
#include "mcld/LD/ELFDynObjReader.h"
#include "mcld/LD/ELFDynObjWriter.h"
#include "mcld/LD/ELFObjectReader.h"
#include "mcld/LD/ELFObjectWriter.h"
#include "mcld/Target/TargetLDBackend.h"

namespace mcld
{

class GNUArchiveReader;
class ELFObjectReader;
class ELFDynObjReader;
class ELFObjectWriter;
class ELFDynObjWriter;

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

  bool initArchiveReader(MCLinker&);
  bool initObjectReader(MCLinker&);
  bool initDynObjReader(MCLinker&);
  bool initObjectWriter(MCLinker&);
  bool initDynObjWriter(MCLinker&);

  ArchiveReader *getArchiveReader();
  ObjectReader *getObjectReader();
  DynObjReader *getDynObjReader();
  ObjectWriter *getObjectWriter();
  DynObjWriter *getDynObjWriter();

protected:
  GNUArchiveReader* m_pArchiveReader;
  ELFObjectReader* m_pObjectReader;
  ELFDynObjReader* m_pDynObjReader;
  ELFObjectWriter* m_pObjectWriter;
  ELFDynObjWriter* m_pDynObjWriter;

};

} // namespace of mcld

#endif


