//===- ELFExecFileFormat.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_EXEC_FILE_FORMAT_H
#define MCLD_ELF_EXEC_FILE_FORMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ELFFileFormat.h>

namespace mcld
{

class GNULDBackend;
class MCLinker;

/** \class ELFExecFileFormat
 *  \brief ELFExecFileFormat describes the format for ELF dynamic objects.
 */
class ELFExecFileFormat : public ELFFileFormat
{
public:
  ELFExecFileFormat(GNULDBackend& pBackend) : ELFFileFormat(pBackend)
  {}

  void initObjectType(MCLinker& pLinker);
};

} // namespace of mcld

#endif

