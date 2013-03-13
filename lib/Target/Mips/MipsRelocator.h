//===- MipsRelocator.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_RELOCATION_FACTORY_H
#define MIPS_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/LD/Relocator.h>
#include <mcld/Support/GCFactory.h>
#include "MipsLDBackend.h"

namespace mcld {

/** \class MipsRelocator
 *  \brief MipsRelocator creates and destroys the Mips relocations.
 */
class MipsRelocator : public Relocator
{
public:
  enum ReservedEntryType {
    None          = 0,  // no reserved entry
    ReserveRel    = 1,  // reserve a dynamic relocation entry
    ReserveGot    = 2,  // reserve a GOT entry
    ReserveGpDisp = 8   // reserve _gp_disp symbol
  };

public:
  MipsRelocator(MipsGNULDBackend& pParent, const LinkerConfig& pConfig);

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  void scanRelocation(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      Module& pModule,
                      LDSection& pSection);

  Result applyRelocation(Relocation& pRelocation, Input* pInput);

  MipsGNULDBackend& getTarget()
  { return m_Target; }

  const MipsGNULDBackend& getTarget() const
  { return m_Target; }

  // Get last calculated AHL.
  int32_t getAHL() const
  { return m_AHL; }

  // Set last calculated AHL.
  void setAHL(int32_t pAHL)
  { m_AHL = pAHL; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

private:
  void scanLocalReloc(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      const LDSection& pSection);

  void scanGlobalReloc(Relocation& pReloc,
                       IRBuilder& pBuilder,
                       const LDSection& pSection);



private:
  MipsGNULDBackend& m_Target;
  int32_t m_AHL;
};

} // namespace of mcld

#endif
