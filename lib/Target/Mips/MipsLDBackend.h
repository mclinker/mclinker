//===- MipsLDBackend.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_LDBACKEND_H
#define MIPS_LDBACKEND_H
#include "mcld/Target/GNULDBackend.h"
#include "MipsGOT.h"

namespace mcld {

class MCLinker;
class SectionMap;

//===----------------------------------------------------------------------===//
/// MipsGNULDBackend - linker backend of Mips target of GNU ELF format
///
class MipsGNULDBackend : public GNULDBackend
{
public:
  MipsGNULDBackend();
  ~MipsGNULDBackend();

  RelocationFactory* getRelocFactory();

  uint32_t machine() const;

  bool isLittleEndian() const;

  unsigned int bitclass() const;

  MipsGOT& getGOT();

  const MipsGOT& getGOT() const;

  bool initTargetSectionMap(SectionMap& pSectionMap);

  void initTargetSections(MCLinker& pLinker);

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  /// (or there are any other target dependent entries for Mips?)
  void scanRelocation(Relocation& pReloc);

private:
  RelocationFactory* m_pRelocFactory;
  MipsGOT* m_pGOT;
};

} // namespace of mcld

#endif

