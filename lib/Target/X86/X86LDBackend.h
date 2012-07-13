//===- X86LDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef X86_LDBACKEND_H
#define X86_LDBACKEND_H

#include "X86ELFDynamic.h"
#include "X86GOT.h"
#include "X86GOTPLT.h"
#include "X86PLT.h"
#include <mcld/LD/LDSection.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Target/OutputRelocSection.h>

namespace mcld {

//===----------------------------------------------------------------------===//
/// X86GNULDBackend - linker backend of X86 target of GNU ELF format
///
class X86GNULDBackend : public GNULDBackend
{
public:
  /** \enum ReservedEntryType
   *  \brief The reserved entry type of reserved space in ResolveInfo.
   *
   *  This is used for sacnRelocation to record what kinds of entries are
   *  reserved for this resolved symbol
   *
   *  In X86, there are three kinds of entries, GOT, PLT, and dynamic reloction.
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

  X86GNULDBackend();

  ~X86GNULDBackend();

  RelocationFactory* getRelocFactory();

  uint32_t machine() const;

  bool isLittleEndian() const
  { return true; }

  X86GOT& getGOT();

  const X86GOT& getGOT() const;

  X86GOTPLT& getGOTPLT();

  const X86GOTPLT& getGOTPLT() const;

  X86PLT& getPLT();

  const X86PLT& getPLT() const;

  unsigned int bitclass() const;

  /// preLayout - Backend can do any needed modification before layout
  void doPreLayout(const Output& pOutput,
                   const MCLDInfo& pInfo,
                   MCLinker& pLinker);

  /// postLayout -Backend can do any needed modification after layout
  void doPostLayout(const Output& pOutput,
                    const MCLDInfo& pInfo,
                    MCLinker& pLinker);

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  X86ELFDynamic& dynamic();

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  const X86ELFDynamic& dynamic() const;

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
  /// @param pLayout - for comouting the size of fragment
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const Output& pOutput,
                           const LDSection& pSection,
                           const MCLDInfo& pInfo,
                           const Layout& pLayout,
                           MemoryRegion& pRegion) const;

  /// OSABI - the value of e_ident[EI_OSABI]
  /// FIXME
  uint8_t OSABI() const
  { return llvm::ELF::ELFOSABI_NONE; }

  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  /// FIXME
  uint8_t ABIVersion() const
  { return 0x0; }

  /// flags - the value of ElfXX_Ehdr::e_flags
  /// FIXME
  uint64_t flags() const
  { return 0x0; }

  uint64_t defaultTextSegmentAddr() const
  { return 0x08048000; }

  /// initTargetSectionMap - initialize target dependent section mapping
  bool initTargetSectionMap(SectionMap& pSectionMap);

  // initRelocFactory - create and initialize RelocationFactory
  bool initRelocFactory(const MCLinker& pLinker);

  void initTargetSections(MCLinker& pLinker);

  void initTargetSymbols(MCLinker& pLinker, const Output& pOutput);

  /// scanRelocation - determine the empty entries are needed or not and create
  /// the empty entries if needed.
  /// For X86, following entries are check to create:
  /// - GOT entry (for .got and .got.plt sections)
  /// - PLT entry (for .plt section)
  /// - dynamin relocation entries (for .rel.plt and .rel.dyn sections)
  void scanRelocation(Relocation& pReloc,
                      const LDSymbol& pInputSym,
                      MCLinker& pLinker,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput,
                      const LDSection& pSection);

  OutputRelocSection& getRelDyn();

  const OutputRelocSection& getRelDyn() const;

  OutputRelocSection& getRelPLT();

  const OutputRelocSection& getRelPLT() const;

  /// getTargetSectionOrder - compute the layout order of X86 target sections
  unsigned int getTargetSectionOrder(const Output& pOutput,
                                     const LDSection& pSectHdr,
                                     const MCLDInfo& pInfo) const;

  /// finalizeTargetSymbols - finalize the symbol value
  bool finalizeTargetSymbols(MCLinker& pLinker, const Output& pOutput);

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

  /// addCopyReloc - add a copy relocation into .rel.dyn for pSym
  /// @param pSym - A resolved copy symbol that defined in BSS section
  void addCopyReloc(ResolveInfo& pSym);

  /// defineSymbolforCopyReloc - allocate a space in BSS section and
  /// and force define the copy of pSym to BSS section
  /// @return the output LDSymbol of the copy symbol
  LDSymbol& defineSymbolforCopyReloc(MCLinker& pLinker,
                                     const ResolveInfo& pSym);

  void updateAddend(Relocation& pReloc,
                    const LDSymbol& pInputSym,
                    const Layout& pLayout) const;

  void createX86GOT(MCLinker& pLinker, const Output& pOutput);
  void createX86GOTPLT(MCLinker& pLinker, const Output& pOutput);
  void createX86PLTandRelPLT(MCLinker& pLinker, const Output& pOutput);
  void createX86RelDyn(MCLinker& pLinker, const Output& pOutput);

private:
  RelocationFactory* m_pRelocFactory;
  X86GOT* m_pGOT;
  X86PLT* m_pPLT;
  X86GOTPLT* m_pGOTPLT;
  /// m_RelDyn - dynamic relocation table of .rel.dyn
  OutputRelocSection* m_pRelDyn;
  /// m_RelPLT - dynamic relocation table of .rel.plt
  OutputRelocSection* m_pRelPLT;

  X86ELFDynamic* m_pDynamic;
  LDSymbol* m_pGOTSymbol;
};

//===----------------------------------------------------------------------===//
/// X86MachOLDBackend - linker backend of X86 target of MachO format
///
/**
class X86MachOLDBackend : public DarwinX86LDBackend
{
public:
  X86MachOLDBackend();
  ~X86MachOLDBackend();

private:
  MCMachOTargetArchiveReader *createTargetArchiveReader() const;
  MCMachOTargetObjectReader *createTargetObjectReader() const;
  MCMachOTargetObjectWriter *createTargetObjectWriter() const;

};
**/
} // namespace of mcld

#endif

