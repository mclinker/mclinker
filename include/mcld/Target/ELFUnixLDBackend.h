/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef ELFUNIXLDBACKEND_H
#define ELFUNIXLDBACKEND_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/TargetLDBackend.h>

namespace mcld
{

class MCArchiveReader;
class MCObjectReader;
class MCObjectWriter;
class MCELFArchiveTargetReader;
class MCELFObjectTargetReader;
class MCELFObjectTargetWriter;

/** \class ELFUnixLDBackend
 *  \brief ELFUnixLDBackend provides a common interface for all Unix-OS LDBackend.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class ELFUnixLDBackend : public TargetLDBackend
{
protected:
  ELFUnixLDBackend();
public:
  virtual ~ELFUnixLDBackend();

  MCArchiveReader *getArchiveReader();
  MCObjectReader *getObjectReader();
  MCObjectWriter *getObjectWriter();

protected:
  virtual MCELFArchiveTargetReader *createArchiveTargetReader() const = 0;
  virtual MCELFObjectTargetReader *createObjectTargetReader() const = 0;
  virtual MCELFObjectTargetWriter *createObjectTargetWriter() const = 0;

private:
  MCObjectReader *m_pObjectReader;
  MCArchiveReader *m_pArchiveReader;
  MCObjectWriter *m_pObjectWriter;
};

} // namespace of mcld

#endif

