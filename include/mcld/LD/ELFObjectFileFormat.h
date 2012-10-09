//===- ELFObjectFileFormat.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_OBJECT_FILE_FROMAT_H
#define MCLD_ELF_OBJECT_FILE_FROMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ELFFileFormat.h>

namespace mcld
{

class GNULDBackend;
class FragmentLinker;

/** \class ELFObjectFileFormat
 *  \brief ELFObjectFileFormat describes the format for ELF dynamic objects.
 */
class ELFObjectFileFormat : public ELFFileFormat
{
public:
  ELFObjectFileFormat(GNULDBackend& pBackend) : ELFFileFormat(pBackend)
  {}

  void initObjectType(FragmentLinker& pLinker);

};

} // namespace of mcld

#endif

