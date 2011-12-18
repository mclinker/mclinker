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
class LDContext;

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

  void initTargetSections(MCLinker& pLinker, LDContext& pContext);

private:
  RelocationFactory* m_pRelocFactory;
  MipsGOT* m_pGOT;
};

} // namespace of mcld

#endif

