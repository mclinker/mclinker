//===- MCBSDArchiveReader.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCELFARCHIVETARGETREADER_H
#define MCELFARCHIVETARGETREADER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class MCELFArchiveTargetReader
 *  \brief MCELFArchiveTargetReader provides an interface for target-dependent archive reader.
 *
 *  \see
 */
class MCELFArchiveTargetReader
{
protected:
  MCELFArchiveTargetReader();

public:
  virtual ~MCELFArchiveTargetReader();

};

} // namespace of mcld

#endif

