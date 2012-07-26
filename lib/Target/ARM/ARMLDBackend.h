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

#include "ARMELFDynamic.h"
#include "ARMGOT.h"
#include "ARMPLT.h"
#include <mcld/LD/LDSection.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Target/OutputRelocSection.h>

namespace mcld {

class MCLDInfo;
class MCLinker;
class Output;
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
  void initTargetSymbols(MCLinker& pLinker, const Output& pOutput);

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
                      const LDSymbol& pInputSym,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput,
                      const LDSection& pSection);

  uint32_t machine() const
  { return llvm::ELF::EM_ARM; }

  /// OSABI - the value of e_ident[EI_OSABI]
  virtual uint8_t OSABI() const
  { return llvm::ELF::ELFOSABI_NONE; }

  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  virtual uint8_t ABIVersion() const
  { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  virtual uint64_t flags() const
  { return (llvm::ELF::EF_ARM_EABIMASK & 0x05000000); }

  bool isLittleEndian() const
  { return true; }

  unsigned int bitclass() const
  { return 32; }

  uint64_t defaultTextSegmentAddr() const
  { return 0x8000; }

  /// doPreLayout - Backend can do any needed modification before layout
  void doPreLayout(const Output& pOutput,
                   const MCLDInfo& pInfo,
                   MCLinker& pLinker);

  /// doPostLayout -Backend can do any needed modification after layout
  void doPostLayout(const Output& pOutput,
                    const MCLDInfo& pInfo,
                    MCLinker& pLinker);

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  ARMELFDynamic& dynamic();

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  const ARMELFDynamic& dynamic() const;


  /// emitSectionData - write out the section data into the memory region.
  /// When writers get a LDSection whose kind is LDFileFormat::Target, writers
  /// call back target backend to emit the data.
  ///
  /// Backends handle the target-special tables (plt, gp,...) by themselves.
  /// Backend can put the data of the tables in SectionData directly
  ///  - LDSection.getSectionData can get the section data.
  /// Or, backend can put the data into special data structure
  ///  - backend can maintain its own map<LDSection, table> to get the table
  /// from given LDSection.
  ///
  /// @param pOutput - the output file
  /// @param pSection - the given LDSection
  /// @param pInfo - all options in the command line.
  /// @param pLayout - for comouting the size of fragment
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const Output& pOutput,
                           const LDSection& pSection,
                           const MCLDInfo& pInfo,
                           const Layout& pLayout,
                           MemoryRegion& pRegion) const;

  ARMGOT& getGOT();

  const ARMGOT& getGOT() const;

  ARMPLT& getPLT();

  const ARMPLT& getPLT() const;

  OutputRelocSection& getRelDyn();

  const OutputRelocSection& getRelDyn() const;

  OutputRelocSection& getRelPLT();

  const OutputRelocSection& getRelPLT() const;

  /// getTargetSectionOrder - compute the layout order of ARM target sections
  unsigned int getTargetSectionOrder(const Output& pOutput,
                                     const LDSection& pSectHdr,
                                     const MCLDInfo& pInfo) const;

  /// finalizeTargetSymbols - finalize the symbol value
  bool finalizeTargetSymbols(MCLinker& pLinker, const Output& pOutput);

  /// readSection - read target dependent sections
  bool readSection(Input& pInput,
                   MCLinker& pLinker,
                   LDSection& pInputSectHdr);

private:
  void scanLocalReloc(Relocation& pReloc,
                      const LDSymbol& pInputSym,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput);

  void scanGlobalReloc(Relocation& pReloc,
                       const LDSymbol& pInputSym,
                       MCLinker& pLinker,
                       const MCLDInfo& pLDInfo,
                       const Output& pOutput);

  void checkValidReloc(Relocation& pReloc,
                       const MCLDInfo& pLDInfo,
                       const Output& pOutput) const;

  /// addCopyReloc - add a copy relocation into .rel.dyn for pSym
  /// @param pSym - A resolved copy symbol that defined in BSS section
  void addCopyReloc(ResolveInfo& pSym);

  /// defineSymbolforCopyReloc - allocate a space in BSS section and
  /// and force define the copy of pSym to BSS section
  /// @return the output LDSymbol of the copy symbol
  LDSymbol& defineSymbolforCopyReloc(MCLinker& pLinker,
                                     const ResolveInfo& pSym);

  /// updateAddend - update addend value of the relocation if the
  /// the target symbol is a section symbol. Addend is the offset
  /// in the section. This value should be updated after section
  /// merged.
  void updateAddend(Relocation& pReloc,
                    const LDSymbol& pInputSym,
                    const Layout& pLayout) const;

  void createARMGOT(MCLinker& pLinker, const Output& pOutput);

  /// createARMPLTandRelPLT - create PLT and RELPLT sections.
  /// Because in ELF sh_info in .rel.plt is the shndx of .plt, these two
  /// sections should be create together.
  void createARMPLTandRelPLT(MCLinker& pLinker, const Output& pOutput);

  void createARMRelDyn(MCLinker& pLinker, const Output& pOutput);

private:
  RelocationFactory* m_pRelocFactory;
  ARMGOT* m_pGOT;
  ARMPLT* m_pPLT;
  /// m_RelDyn - dynamic relocation table of .rel.dyn
  OutputRelocSection* m_pRelDyn;
  /// m_RelPLT - dynamic relocation table of .rel.plt
  OutputRelocSection* m_pRelPLT;

  ARMELFDynamic* m_pDynamic;
  LDSymbol* m_pGOTSymbol;

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

