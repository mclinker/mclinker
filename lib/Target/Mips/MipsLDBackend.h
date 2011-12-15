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
#include "MipsRelocationFactory.h"

namespace mcld {

//===----------------------------------------------------------------------===//
/// MipsGNULDBackend - linker backend of Mips target of GNU ELF format
///
class MipsGNULDBackend : public GNULDBackend
{
public:
  MipsGNULDBackend();
  ~MipsGNULDBackend();

  /// getRelocFactory - co-variant return type
  MipsRelocationFactory* getRelocFactory();

  uint32_t machine() const;

private:
  MipsRelocationFactory* m_pRelocFactory;
};

} // namespace of mcld

#endif

