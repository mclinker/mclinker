/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Duo <pinronglu@gmail.com>                                               *
 ****************************************************************************/
#ifndef MCOBJECTREADER_H
#define MCOBJECTREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/TargetLDBackend.h>

namespace mcld
{

class TargetObjectReader;

/** \class MCObjectReader
 *  \brief MCObjectReader provides an interface for different object formats.
 *
 *  \see
 *  \author Duo <pinronglu@gmail.com>
 */
class MCObjectReader
{
public:
  MCObjectReader(TargetLDBackend::TargetObjectReaderCtorFnTy pCtorFn);
  virtual ~MCObjectReader();

private:
  TargetObjectReader *m_pReader;

};

} // namespace of mcld

#endif

