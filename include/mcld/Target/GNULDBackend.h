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

class MCLDInfo;
class Layout;

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

  /// sizeNamePools - compute the size of regular name pools
  /// In ELF executable files, regular name pools are .symtab, .strtab.,
  /// .dynsym, .dynstr, and .hash
  virtual void sizeNamePools(const Output& pOutput,
                             const MCLDInfo& pLDInfo);

  /// emitSectionData - emit target-dependent section data
  virtual uint64_t emitSectionData(const Output& pOutput,
                                   const LDSection& pSection,
                                   const MCLDInfo& pInfo,
                                   MemoryRegion& pRegion) const = 0;

  /// emitRegNamePools - emit regular name pools - .symtab, .strtab
  virtual void emitRegNamePools(Output& pOutput,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);
  /// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
  virtual void emitDynNamePools(Output& pOutput,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);

  /// getSectionOrder - compute the layout order of the section
  /// Layout calls this function to get the default order of the pSectHdr.
  /// If the pSectHdr.type() is LDFileFormat::Target, then getSectionOrder()
  /// will call getTargetSectionOrder().
  ///
  /// If targets favors certain order for general sections, please override
  /// this function.
  ///
  /// @see getTargetSectionOrder
  virtual unsigned int getSectionOrder(const LDSection& pSectHdr) const;

  /// getTargetSectionOrder - compute the layout order of target section
  /// If the target favors certain order for the given gSectHdr, please
  /// override this function.
  ///
  /// By default, this function returns the maximun order, and pSectHdr
  /// will be the last section to be laid out.
  virtual unsigned int
  getTargetSectionOrder(const LDSection& pSectHdr) const
  { return (unsigned int)-1; }

protected:
  uint64_t getSymbolInfo(const LDSymbol& pSymbol) const;

  uint64_t getSymbolShndx(const LDSymbol& pSymbol, const Layout& pLayout) const;

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

