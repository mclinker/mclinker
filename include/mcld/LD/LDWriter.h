//===- LDWriter.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
//  LDWriter provides an interface used by MCLinker,
//  which writes the result of linking into a .so file or a executable.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_WRITER_INTERFACE_H
#define MCLD_WRITER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Target/TargetLDBackend.h>

namespace mcld
{

/** \class LDWriter
 *  \brief LDWriter provides the basic interfaces for all writers.
 *  (ObjectWriter, DynObjWriter, and EXEObjWriter)
 */
class LDWriter
{
protected:
  LDWriter() { }

public:
  virtual ~LDWriter() { }

};

} //end namespace

#endif

