//===- ELFSectionMap.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_ELF_SECTION_MAP_H
#define MCLD_LD_ELF_SECTION_MAP_H

#include <mcld/LD/SectionMap.h>

namespace mcld
{

class ELFSectionMap : public SectionMap
{
public:
  ELFSectionMap();

  ~ELFSectionMap();

  void initStandardMaps();
};

} // namespace of mcld

#endif

