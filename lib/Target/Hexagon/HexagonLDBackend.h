//===- HexagonLDBackend.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef HEXAGON_LDBACKEND_H
#define HEXAGON_LDBACKEND_H

#include "HexagonELFDynamic.h"
#include "HexagonGOT.h"
#include "HexagonPLT.h"
#include <mcld/LD/LDSection.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Target/OutputRelocSection.h>

namespace mcld {

class LinkerConfig;
class HexagonGNUInfo;

//===----------------------------------------------------------------------===//
/// HexagonLDBackend - linker backend of Hexagon target of GNU ELF format
///
class HexagonLDBackend : public GNULDBackend
{
public:
  HexagonLDBackend(const LinkerConfig& pConfig, HexagonGNUInfo* pInfo);

  ~HexagonLDBackend();

  uint32_t machine() const;

  HexagonGOT& getGOT();

  const HexagonGOT& getGOT() const;

  HexagonPLT& getPLT();

  const HexagonPLT& getPLT() const;

  /// preLayout - Backend can do any needed modification before layout
  void doPreLayout(IRBuilder& pBuilder);

  /// postLayout - Backend can do any needed modification after layout
  void doPostLayout(Module& pModule, IRBuilder& pBuilder);

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  HexagonELFDynamic& dynamic();

  /// dynamic - the dynamic section of the target machine.
  /// Use co-variant return type to return its own dynamic section.
  const HexagonELFDynamic& dynamic() const;

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
  /// @param pSection - the given LDSection
  /// @param pLayout - for comouting the size of fragment
  /// @param pRegion - the region to write out data
  /// @return the size of the table in the file.
  uint64_t emitSectionData(const LDSection& pSection,
                           MemoryRegion& pRegion) const;

  /// initRelocator - create and initialize Relocator.
  bool initRelocator();

  /// getRelocator - return relocator.
  Relocator* getRelocator();

  ResolveInfo::Desc getSymDesc(uint16_t shndx) const
  {
    if (shndx >= llvm::ELF::SHN_HEXAGON_SCOMMON &&
        shndx <= llvm::ELF::SHN_HEXAGON_SCOMMON_8)
      return ResolveInfo::Common;
    return ResolveInfo::NoneDesc;
  }

  void initTargetSections(Module& pModule, ObjectBuilder& pBuilder);

  void initTargetSymbols(IRBuilder& pBuilder, Module& pModule);

  /// scanRelocation - determine the empty entries are needed or not and create
  /// the empty entries if needed.
  /// For Hexagon, following entries are check to create:
  /// - GOT entry (for .got and .got.plt sections)
  /// - PLT entry (for .plt section)
  /// - dynamin relocation entries (for .rel.plt and .rel.dyn sections)
  void scanRelocation(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      Module& pModule,
                      LDSection& pSection);

  OutputRelocSection& getRelDyn();

  const OutputRelocSection& getRelDyn() const;

  OutputRelocSection& getRelPLT();

  const OutputRelocSection& getRelPLT() const;

  /// getTargetSectionOrder - compute the layout order of Hexagon target section
  unsigned int getTargetSectionOrder(const LDSection& pSectHdr) const;

  /// finalizeTargetSymbols - finalize the symbol value
  bool finalizeTargetSymbols();

private:

  /// getRelEntrySize - the size in BYTE of rel type relocation
  size_t getRelEntrySize()
  { return 8; }

  /// getRelEntrySize - the size in BYTE of rela type relocation
  size_t getRelaEntrySize()
  { return 12; }

  /// doCreateProgramHdrs - backend can implement this function to create the
  /// target-dependent segments
  void doCreateProgramHdrs(Module& pModule);

private:
  Relocator* m_pRelocator;
  HexagonGOT* m_pGOT;
  HexagonPLT* m_pPLT;
  /// m_RelDyn - dynamic relocation table of .rel.dyn
  OutputRelocSection* m_pRelDyn;
  /// m_RelPLT - dynamic relocation table of .rel.plt
  OutputRelocSection* m_pRelPLT;

  HexagonELFDynamic* m_pDynamic;
  LDSymbol* m_pGOTSymbol;
  LDSymbol* m_pBSSEnd;
};
} // namespace of mcld

#endif

