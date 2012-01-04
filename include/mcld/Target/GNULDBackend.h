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
  // These dynamic section tags are GNU extension.
  enum {
    DT_RELACOUNT  = 0x6ffffff9,
    DT_RELCOUNT   = 0x6ffffffa,
    DT_FLAGS_1    = 0x6ffffffb,
    DT_VERDEF     = 0x6ffffffc,
    DT_VERDEFNUM  = 0x6ffffffd,
    DT_VERNEED    = 0x6ffffffe,
    DT_VERNEEDNUM = 0x6fffffff
  };

protected:
  // Based on Kind in LDFileFormat to define basic section orders for ELF, and
  // refer gold linker to add more enumerations to handle Regular and BSS kind
  enum SectionOrder {
    SHO_INTERP = 1,          // .interp
    SHO_RO_NOTE,             // .note.ABI-tag, .note.gnu.build-id
    SHO_NAMEPOOL,            // *.hash, .dynsym, .dynstr
    SHO_RELOCATION,          // .rel.*, .rela.*
    SHO_REL_PLT,             // .rel.plt should come after other .rel.*
    SHO_INIT,                // .init
    SHO_PLT,                 // .plt
    SHO_TEXT,                // .text
    SHO_FINI,                // .fini
    SHO_RO,                  // .rodata
    SHO_EHFRAME,             // .eh_frame_hdr, .eh_frame
    SHO_TLS_DATA,            // .tdata
    SHO_TLS_BSS,             // .tbss
    SHO_RELRO_LOCAL,         // .data.rel.ro.local
    SHO_RELRO,               // .data.rel.ro,
    SHO_RELRO_LAST,          // for x86 to adjust .got if needed
    SHO_NON_RELRO_FIRST,     // for x86 to adjust .got.plt if needed
    SHO_DATA,                // .data
    SHO_LARGE_DATA,          // .ldata
    SHO_RW_NOTE,             //
    SHO_SMALL_DATA,          // .sdata
    SHO_SMALL_BSS,           // .sbss
    SHO_BSS,                 // .bss
    SHO_LARGE_BSS,           // .lbss
    SHO_UNDEFINED = ~(0U)    // default order
  };

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

  /// The return value of machine() it the same as e_machine in the ELF header*/
  virtual uint32_t machine() const = 0;

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

  // sizeDynamic - compute the size of .dynamic section
  virtual void sizeDynamic(Output& pOutput,
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

  /// emitDynamic - emit .dynamic section
  virtual void emitDynamic(Output& pOutput,
                           const Layout& pLayout);

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

  virtual unsigned int numOfSegments() const {
    // FIXME:
    return 10;
  }
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

