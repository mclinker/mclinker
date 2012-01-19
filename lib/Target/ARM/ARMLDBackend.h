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
#include "ARMDynRelSection.h"
#include <mcld/Target/GNULDBackend.h>
#include <mcld/LD/LDSection.h>

namespace mcld {

class Output;
class MCLDInfo;
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
public:
  typedef std::vector<llvm::ELF::Elf32_Dyn*> ELF32DynList;

  /** \enum ReservedEntryType
   *  \brief The reserved entry type of reserved space in ResolveInfo.
   *
   *  This is used for sacnRelocation to record what kinds of entries are
   *  reserved for this resolved symbol
   *
   *  In ARM, there are three kinds of entries, GOT, PLT, and dynamic reloction.
   *  GOT may needs a corresponding relocation to relocate itself, so we
   *  separate GOT to two situations: GOT and GOTRel. Besides, for the same
   *  symbol, there might be two kinds of entries reserved for different location.
   *  For example, reference to the same symbol, one may use GOT and the other may
   *  use dynamic relocation.
   *
   *  bit:  3       2      1     0
   *   | PLT | GOTRel | GOT | Rel |
   *
   *  value    Name         - Description
   *
   *  0000     None         - no reserved entry
   *  0001     ReserveRel   - reserve an dynamic relocation entry
   *  0010     ReserveGOT   - reserve an GOT entry
   *  0011     GOTandRel    - For different relocation, we've reserved GOT and
   *                          Rel for different location.
   *  0100     GOTRel       - reserve an GOT entry and the corresponding Dyncamic
   *                          relocation entry which relocate this GOT entry
   *  0101     GOTRelandRel - For different relocation, we've reserved GOTRel
   *                          and relocation entry for different location.
   *  1000     ReservePLT   - reserve an PLT entry and the corresponding GOT,
   *                          Dynamic relocation entries
   *  1001     PLTandRel    - For different relocation, we've reserved PLT and
   *                          Rel for different location.
   */
  enum ReservedEntryType {
    None         = 0,
    ReserveRel   = 1,
    ReserveGOT   = 2,
    GOTandRel    = 3,
    GOTRel       = 4,
    GOTRelandRel = 5,
    ReservePLT   = 8,
    PLTandRel    = 9
  };

public:
  /// initTargetSectionMap - initialize target dependent section mapping
  bool initTargetSectionMap(SectionMap& pSectionMap);

  /// initTargetSections - initialize target dependent sections in output.
  void initTargetSections(MCLinker& pLinker);

  /// initTargetSymbols - initialize target dependent symbols in output.
  void initTargetSymbols(MCLinker& pLinker);
  
  /// initRelocFactory - create and initialize RelocationFactory
  bool initRelocFactory(const MCLinker& pLinker);

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
                      const MCLDInfo& pLDInfo,
                      unsigned int pType);

  uint32_t machine() const;

  /// OSABI - the value of e_ident[EI_OSABI]
  virtual uint8_t OSABI() const
  { return llvm::ELF::ELFOSABI_NONE; }
  
  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  virtual uint8_t ABIVersion() const
  { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  virtual uint64_t flags() const
  { return (llvm::ELF::EF_ARM_EABIMASK & 0x05000000); }

  bool isLittleEndian() const;

  unsigned int bitclass() const
  { return 32; }

  /// doPreLayout - Backend can do any needed modification before layout
  void doPreLayout(const Output& pOutput,
                   const MCLDInfo& pInfo,
                   MCLinker& pLinker);

  /// doPostLayout -Backend can do any needed modification after layout
  void doPostLayout(const Output& pOutput,
                    const MCLDInfo& pInfo,
                    MCLinker& pLinker);

  /// emitSectionData - write out the section data into the memory region.
  /// When writers get a LDSection whose kind is LDFileFormat::Target, writers
  /// call back target backend to emit the data.
  ///
  /// Backends handle the target-special tables (plt, gp,...) by themselves.
  /// Backend can put the data of the tables in MCSectionData directly
  ///  - LDSection.getSectionData can get the section data.
  /// Or, backend can put the data into special data structure
  ///  - backend can maintain its own map<LDSection, table> to get the table
  /// from given LDSection.
  ///
  /// @param pOutput - the output file
  /// @param pSection - the given LDSection
  /// @param pInfo - all options in the command line.
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const Output& pOutput,
                           const LDSection& pSection,
                           const MCLDInfo& pInfo,
                           MemoryRegion& pRegion) const;

  ARMGOT& getGOT();

  const ARMGOT& getGOT() const;

  ARMPLT& getPLT();

  const ARMPLT& getPLT() const;

  ARMDynRelSection& getRelDyn();

  const ARMDynRelSection& getRelDyn() const;

  ARMDynRelSection& getRelPLT();

  const ARMDynRelSection& getRelPLT() const;

  /// getTargetSectionOrder - compute the layout order of ARM target sections
  unsigned int getTargetSectionOrder(const LDSection& pSectHdr) const;

  /// finalizeSymbol - finalize the symbol value
  /// If the symbol's reserved field is not zero, MCLinker will call back this
  /// function to ask the final value of the symbol
  bool finalizeSymbol(LDSymbol& pSymbol) const;

private:
  void scanLocalReloc(Relocation& pReloc,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      unsigned int pType);

  void scanGlobalReloc(Relocation& pReloc,
                       MCLinker& pLinker,
                       const MCLDInfo& pLDInfo,
                       unsigned int pType);

  bool isSymbolNeedsPLT(const ResolveInfo& pSym, 
                        unsigned int pType, 
                        const MCLDInfo& pLDInfo);

  bool isSymbolNeedsDynRel(const ResolveInfo& pSym, 
                           unsigned int pType, 
                           bool isAbsReloc);

  bool isSymbolPreemptible(const ResolveInfo& pSym,
                          unsigned int pType,
                          const MCLDInfo& pLDInfo);

  void checkValidReloc(Relocation& pReloc, unsigned int pOutputType);
  void createARMGOT(MCLinker& pLinker, unsigned int pType);
  void createARMPLTandRelPLT(MCLinker& pLinker, unsigned int pType);
  void createARMRelDyn(MCLinker& pLinker, unsigned int pType);

private:
  RelocationFactory* m_pRelocFactory;
  ARMGOT* m_pGOT;
  ARMPLT* m_pPLT;
  /// m_RelDyn - dynamic relocation table of .rel.dyn
  ARMDynRelSection* m_pRelDyn;
  /// m_RelPLT - dynamic relocation table of .rel.plt
  ARMDynRelSection* m_pRelPLT;

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

