//===- LDFileFormat.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LDFILE_FORMAT_H
#define MCLD_LDFILE_FORMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class LDFileFormat
 *  \brief LDFileFormat describes the common file formats.
 */
class LDFileFormat
{
public:
  enum Kind {
    GOT
  };

protected:
  LDFileFormat();

public:
  virtual ~LDFileFormat();

};

} // namespace of mcld

#endif

