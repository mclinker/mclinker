//===- GNULDBackend.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_TARGET_GNU_LDBACKEND_H
#define MCLD_TARGET_GNU_LDBACKEND_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/ELF.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/LD/GNUArchiveReader.h>
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/LD/ELFDynObjWriter.h>
#include <mcld/LD/ELFObjectReader.h>
#include <mcld/LD/ELFObjectWriter.h>
#include <mcld/LD/ELFDynObjFileFormat.h>
#include <mcld/LD/ELFExecFileFormat.h>

namespace mcld
{

/** \class GNULDBackend
 *  \brief GNULDBackend provides a common interface for all GNU Unix-OS
 *  LDBackend.
 */
class GNULDBackend : public TargetLDBackend
{
protected:
  GNULDBackend();

public:
  virtual ~GNULDBackend();

  bool initArchiveReader(MCLinker& pLinker);
  bool initObjectReader(MCLinker& pLinker);
  bool initDynObjReader(MCLinker& pLinker);
  bool initObjectWriter(MCLinker& pLinker);
  bool initDynObjWriter(MCLinker& pLinker);

  bool initExecSections(MCLinker& pMCLinker);
  bool initDynObjSections(MCLinker& pMCLinker);

  GNUArchiveReader *getArchiveReader();
  ELFObjectReader *getObjectReader();
  ELFDynObjReader *getDynObjReader();
  ELFObjectWriter *getObjectWriter();
  ELFDynObjWriter *getDynObjWriter();

  ELFDynObjFileFormat* getDynObjFileFormat();
  ELFExecFileFormat* getExecFileFormat();

  size_t sectionStartOffset() const;

  /// ELFVersion - the value of e_ident[EI_VERSION]
  virtual uint8_t ELFVersion() const
  { return llvm::ELF::EV_CURRENT; }

  /// OSABI - the value of e_ident[EI_OSABI]
  virtual uint8_t OSABI() const = 0;

  /// ABIVersion - the value of e_ident[EI_ABIVRESION]
  virtual uint8_t ABIVersion() const = 0;

  /// flags - the value of ElfXX_Ehdr::e_flags
  virtual uint64_t flags() const = 0;

  /// entry - the symbol name of the entry point
  virtual const char* entry() const
  { return "_start"; }

  /// emitSectionData - emit target-dependent section data
  virtual uint64_t emitSectionData(const Output& pOutput,
                                   const LDSection& pSection,
                                   const MCLDInfo& pInfo,
                                   MemoryRegion& pRegion) const = 0;

  /// emitRegNamePools - emit regular name pools - .symtab, .strtab
  virtual uint64_t emitRegNamePools(Output& pOutput,
                                    const MCLDInfo& pLDInfo) const;
  /// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
  virtual uint64_t emitDynNamePools(Output& pOutput,
                                    const MCLDInfo& pLDInfo) const;

  /// computeSectionOrder - compute the layout order of the section
  unsigned int computeSectionOrder(const LDSection& pSectHdr) const;

  /// computeTargetSectionOrder - compute the layout order of target section
  virtual unsigned int computeTargetSectionOrder(
    const LDSection& pSectHdr) const = 0;

protected:
  // ----- readers and writers ----- //
  GNUArchiveReader* m_pArchiveReader;
  ELFObjectReader* m_pObjectReader;
  ELFDynObjReader* m_pDynObjReader;
  ELFObjectWriter* m_pObjectWriter;
  ELFDynObjWriter* m_pDynObjWriter;

  // -----  file formats  ----- //
  ELFDynObjFileFormat* m_pDynObjFileFormat;
  ELFExecFileFormat* m_pExecFileFormat;

};

} // namespace of mcld

#endif

