//===- GNULDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_GNU_LDBACKEND_H
#define MCLD_GNU_LDBACKEND_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Target/TargetLDBackend.h>
#include <mcld/LD/GNUArchiveReader.h>
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/LD/ELFDynObjWriter.h>
#include <mcld/LD/ELFObjectReader.h>
#include <mcld/LD/ELFObjectWriter.h>
#include <mcld/LD/ELFDynObjFileFormat.h>
#include <mcld/LD/ELFExecFileFormat.h>

namespace mcld
{

class LDContext;

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

  bool initArchiveReader(MCLinker& pLinker);
  bool initObjectReader(MCLinker& pLinker);
  bool initDynObjReader(MCLinker& pLinker);
  bool initObjectWriter(MCLinker& pLinker);
  bool initDynObjWriter(MCLinker& pLinker);

  bool initExecSections(MCLinker& pMCLinker, LDContext& pContext);
  bool initDynObjSections(MCLinker& pMCLinker, LDContext& pContext);

  GNUArchiveReader *getArchiveReader();
  ELFObjectReader *getObjectReader();
  ELFDynObjReader *getDynObjReader();
  ELFObjectWriter *getObjectWriter();
  ELFDynObjWriter *getDynObjWriter();

  ELFDynObjFileFormat* getDynObjFileFormat();
  ELFExecFileFormat* getExecFileFormat();

protected:
  // ----- readers and writers ----- //
  GNUArchiveReader* m_pArchiveReader;
  ELFObjectReader* m_pObjectReader;
  ELFDynObjReader* m_pDynObjReader;
  ELFObjectWriter* m_pObjectWriter;
  ELFDynObjWriter* m_pDynObjWriter;

  // -----  file formats  ----- //
  ELFDynObjFileFormat* m_pDynObjFileFormat;
  ELFExecFileFormat* m_pExecFileFormat;

};

} // namespace of mcld

#endif


