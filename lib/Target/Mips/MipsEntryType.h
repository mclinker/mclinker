//===- MipsEntryType.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_ENTRYTYPE_H
#define MIPS_ENTRYTYPE_H

namespace mcld {

//===----------------------------------------------------------------------===//
/// MipsReservedEntryType - entry reservation flags.
///
enum MipsReservedEntryType {
  None          = 0,  // no reserved entry
  ReserveRel    = 1,  // reserve a dynamic relocation entry
  ReserveGot    = 4,  // reserve a GOT entry
  ReserveGpDisp = 8   // reserve _gp_disp symbol
};

} // namespace of mcld

#endif

