//===- ELFDynObjFileFormat.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_DYNAMIC_OBJECT_FILE_FROMAT_H
#define MCLD_ELF_DYNAMIC_OBJECT_FILE_FROMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ELFFileFormat.h>

namespace mcld
{

class GNULDBackend;
class MCLinker;

/** \class ELFDynObjFileFormat
 *  \brief ELFDynObjFileFormat describes the format for ELF dynamic objects.
 */
class ELFDynObjFileFormat : public ELFFileFormat
{
public:
  ELFDynObjFileFormat(GNULDBackend& pBackend) : ELFFileFormat(pBackend)
  {}

  void initObjectType(MCLinker& pLinker);

};

} // namespace of mcld

#endif

