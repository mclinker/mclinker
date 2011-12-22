//===- ARMLDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ARM_LDBACKEND_H
#define MCLD_ARM_LDBACKEND_H

#include "ARMGOT.h"
#include "ARMPLT.h"
#include <mcld/Target/GNULDBackend.h>
#include <mcld/LD/LDSection.h>

namespace mcld {

class MCLinker;
class SectionMap;

//===----------------------------------------------------------------------===//
/// ARMGNULDBackend - linker backend of ARM target of GNU ELF format
///
class ARMGNULDBackend : public GNULDBackend
{
public:
  ARMGNULDBackend();
  ~ARMGNULDBackend();

  /// initTargetSectionMap - initialize target dependent section mapping
  bool initTargetSectionMap(SectionMap& pSectionMap);

  /// initTargetSections - initialize target dependent sections in output.
  void initTargetSections(MCLinker& pLinker);

  /// getRelocFactory
  RelocationFactory* getRelocFactory();

  /// scanRelocation - determine the empty entries are needed or not and create
  /// the empty entries if needed.
  /// For ARM, following entries are check to create:
  /// - GOT entry (for .got section)
  /// - PLT entry (for .plt section)
  /// - dynamin relocation entries (for .rel.plt and .rel.dyn sections)
  void scanRelocation(Relocation& pReloc,
                      MCLinker& pLinker,
                      unsigned int pType);

  uint32_t machine() const;

  bool isLittleEndian() const;

  unsigned int bitclass() const
  { return 32; }

  ARMGOT& getGOT();
  const ARMGOT& getGOT() const;

  ARMPLT& getPLT();
  const ARMPLT& getPLT() const;

  llvm::MCSectionData& getRelDyn();
  const llvm::MCSectionData& getRelDyn() const;

  llvm::MCSectionData& getRelPLT();
  const llvm::MCSectionData& getRelPLT() const;

private:
  RelocationFactory* m_pRelocFactory;
  ARMGOT* m_pGOT;
  ARMPLT* m_pPLT;
  /// m_RelDyn - dynamic relocation table of .rel.dyn
  llvm::MCSectionData* m_pRelDyn;
  /// m_RelPLT - dynamic relocation table of .rel.plt
  llvm::MCSectionData* m_pRelPLT;

  //     variable name           :  ELF
  LDSection* m_pEXIDX;           // .ARM.exidx
  LDSection* m_pEXTAB;           // .ARM.extab
  LDSection* m_pAttributes;      // .ARM.attributes
//  LDSection* m_pPreemptMap;      // .ARM.preemptmap
//  LDSection* m_pDebugOverlay;    // .ARM.debug_overlay
//  LDSection* m_pOverlayTable;    // .ARM.overlay_table
};

//===----------------------------------------------------------------------===//
/// ARMMachOLDBackend - linker backend of ARM target of MachO format
///
/**
class ARMMachOLDBackend : public DarwinARMLDBackend
{
public:
  ARMMachOLDBackend();
  ~ARMMachOLDBackend();

private:
  MCMachOTargetArchiveReader *createTargetArchiveReader() const;
  MCMachOTargetObjectReader *createTargetObjectReader() const;
  MCMachOTargetObjectWriter *createTargetObjectWriter() const;

};
**/
} // namespace of mcld

#endif

