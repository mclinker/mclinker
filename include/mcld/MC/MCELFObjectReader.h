/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCELFOBJECTREADER_H
#define MCELFOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/MC/MCObjectReader.h>

namespace mcld
{
class MCELFObjectTargetReader;

/** \class MCELFObjectReader
 *  \brief MCELFObjectReader reads target-independent parts of ELF object file.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCELFObjectReader : public MCObjectReader
{
public:
  MCELFObjectReader(const MCELFObjectTargetReader *pTargetReader);
  ~MCELFObjectReader();

private:
  const MCELFObjectTargetReader *m_pTargetReader;
};

} // namespace of mcld

#endif

