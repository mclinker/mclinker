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
#include <mcld/ADT/HashTable.h>
#include <mcld/ADT/HashEntry.h>
#include <mcld/LD/EhFrameHdr.h>
#include <mcld/LD/ELFDynObjFileFormat.h>
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/LD/ELFDynObjWriter.h>
#include <mcld/LD/ELFExecFileFormat.h>
#include <mcld/LD/ELFExecWriter.h>
#include <mcld/LD/ELFObjectReader.h>
#include <mcld/LD/ELFObjectWriter.h>
#include <mcld/LD/ELFSegment.h>
#include <mcld/LD/ELFSegmentFactory.h>
#include <mcld/LD/GNUArchiveReader.h>
#include <mcld/Support/GCFactory.h>
#include <mcld/Target/ELFDynamic.h>
#include <mcld/Target/TargetLDBackend.h>

namespace mcld
{

struct SymCompare
{
  bool operator()(const LDSymbol* X, const LDSymbol* Y) const
  { return (X==Y); }
};

struct PtrHash
{
  size_t operator()(const LDSymbol* pKey) const
  {
    return (unsigned((uintptr_t)pKey) >> 4) ^
           (unsigned((uintptr_t)pKey) >> 9);
  }
};

class MCLDInfo;
class Layout;
class SymbolCategory;

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

  // -----  readers/writers  ----- //
  bool initArchiveReader(MCLinker& pLinker,
                         MCLDInfo& pInfo,
                         MemoryAreaFactory& pMemAreaFactory);
  bool initObjectReader(MCLinker& pLinker);
  bool initDynObjReader(MCLinker& pLinker);
  bool initObjectWriter(MCLinker& pLinker);
  bool initDynObjWriter(MCLinker& pLinker);
  bool initExecWriter(MCLinker& pLinker);

  GNUArchiveReader *getArchiveReader();
  const GNUArchiveReader *getArchiveReader() const;

  ELFObjectReader *getObjectReader();
  const ELFObjectReader *getObjectReader() const;

  ELFDynObjReader *getDynObjReader();
  const ELFDynObjReader *getDynObjReader() const;

  ELFObjectWriter *getObjectWriter();
  const ELFObjectWriter *getObjectWriter() const;

  ELFDynObjWriter *getDynObjWriter();
  const ELFDynObjWriter *getDynObjWriter() const;

  ELFExecWriter *getExecWriter();
  const ELFExecWriter *getExecWriter() const;

  // -----  output sections  ----- //
  /// initExecSections - initialize sections of the output executable file.
  bool initExecSections(MCLinker& pMCLinker);

  /// initDynObjSections - initialize sections of the output shared object.
  bool initDynObjSections(MCLinker& pMCLinker);

  /// getOutputFormat - get the sections of the output file.
  ELFFileFormat* getOutputFormat(const Output& pOutput);
  const ELFFileFormat* getOutputFormat(const Output& pOutput) const;

  ELFDynObjFileFormat* getDynObjFileFormat();
  const ELFDynObjFileFormat* getDynObjFileFormat() const;

  ELFExecFileFormat* getExecFileFormat();
  const ELFExecFileFormat* getExecFileFormat() const;

  // -----  target symbols ----- //
  /// initStandardSymbols - initialize standard symbols.
  /// Some section symbols is undefined in input object, and linkers must set
  /// up its value. Take __init_array_begin for example. This symbol is an
  /// undefined symbol in input objects. MCLinker must finalize its value
  /// to the begin of the .init_array section, then relocation enties to
  /// __init_array_begin can be applied without emission of "undefined
  /// reference to `__init_array_begin'".
  bool initStandardSymbols(MCLinker& pLinker, const Output& pOutput);

  /// finalizeSymbol - Linker checks pSymbol.reserved() if it's not zero,
  /// then it will ask backend to finalize the symbol value.
  /// @return ture - if backend set the symbol value sucessfully
  /// @return false - if backend do not recognize the symbol
  bool finalizeSymbols(MCLinker& pLinker, const Output& pOutput) {
    return (finalizeStandardSymbols(pLinker, pOutput) &&
            finalizeTargetSymbols(pLinker, pOutput));
  }

  /// finalizeStandardSymbols - set the value of standard symbols
  virtual bool finalizeStandardSymbols(MCLinker& pLinker,
                                       const Output& pOutput);

  /// finalizeTargetSymbols - set the value of target symbols
  virtual bool finalizeTargetSymbols(MCLinker& pLinker,
                                     const Output& pOutput) = 0;

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

  /// dyld - the name of the default dynamic linker
  /// target may override this function if needed.
  /// @ref gnu ld, bfd/elf32-i386.c:521
  virtual const char* dyld() const
  { return "/usr/lib/libc.so.1"; }

  /// defaultTextSegmentAddr - target should specify its own default start address
  /// of the text segment. esp. for exec.
  virtual uint64_t defaultTextSegmentAddr() const
  { return 0x0; }

  /// segmentStartAddr - this function returns the start address of the segment
  uint64_t segmentStartAddr(const Output& pOutput,
                            const MCLDInfo& pInfo) const;

  /// sizeNamePools - compute the size of regular name pools
  /// In ELF executable files, regular name pools are .symtab, .strtab.,
  /// .dynsym, .dynstr, and .hash
  virtual void sizeNamePools(const Output& pOutput,
                             const SymbolCategory& pSymbols,
                             const MCLDInfo& pLDInfo);

  /// emitSectionData - emit target-dependent section data
  virtual uint64_t emitSectionData(const Output& pOutput,
                                   const LDSection& pSection,
                                   const MCLDInfo& pInfo,
                                   const Layout& pLayout,
                                   MemoryRegion& pRegion) const = 0;

  /// emitRegNamePools - emit regular name pools - .symtab, .strtab
  virtual void emitRegNamePools(Output& pOutput,
                                SymbolCategory& pSymbols,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);

  /// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
  virtual void emitDynNamePools(Output& pOutput,
                                SymbolCategory& pSymbols,
                                const Layout& pLayout,
                                const MCLDInfo& pLDInfo);

  /// sizeInterp - compute the size of program interpreter's name
  /// In ELF executables, this is the length of dynamic linker's path name
  virtual void sizeInterp(const Output& pOutput, const MCLDInfo& pLDInfo);

  /// emitInterp - emit the .interp
  virtual void emitInterp(Output& pOutput, const MCLDInfo& pLDInfo);

  /// getSectionOrder - compute the layout order of the section
  /// Layout calls this function to get the default order of the pSectHdr.
  /// If the pSectHdr.type() is LDFileFormat::Target, then getSectionOrder()
  /// will call getTargetSectionOrder().
  ///
  /// If targets favors certain order for general sections, please override
  /// this function.
  ///
  /// @see getTargetSectionOrder
  virtual unsigned int getSectionOrder(const Output& pOutput,
                                       const LDSection& pSectHdr,
                                       const MCLDInfo& pInfo) const;

  /// getTargetSectionOrder - compute the layout order of target section
  /// If the target favors certain order for the given gSectHdr, please
  /// override this function.
  ///
  /// By default, this function returns the maximun order, and pSectHdr
  /// will be the last section to be laid out.
  virtual unsigned int
  getTargetSectionOrder(const Output& pOutput,
                        const LDSection& pSectHdr,
                        const MCLDInfo& pInfo) const
  { return (unsigned int)-1; }

  /// numOfSegments - return the number of segments
  /// if the target favors other ways to emit program header, please override
  /// this function
  virtual unsigned int numOfSegments() const
  { return m_ELFSegmentTable.size(); }

  /// elfSegmentTable - return the reference of the elf segment table
  ELFSegmentFactory& elfSegmentTable()
  { return m_ELFSegmentTable; }

  /// elfSegmentTable - return the reference of the elf segment table
  const ELFSegmentFactory& elfSegmentTable() const
  { return m_ELFSegmentTable; }

  /// commonPageSize - the common page size of the target machine, and we set it
  /// to 4K here. If target favors the different size, please override this
  /// function
  virtual uint64_t commonPageSize(const MCLDInfo& pInfo) const;

  /// abiPageSize - the abi page size of the target machine, and we set it to 4K
  /// here. If target favors the different size, please override this function
  virtual uint64_t abiPageSize(const MCLDInfo& pInfo) const;

  /// getSymbolIdx - get the symbol index of ouput symbol table
  size_t getSymbolIdx(LDSymbol* pSymbol) const;

  /// isDefaultExecStack - target should specify whether the stack is default
  /// executable. If target favors another choice, please override this function
  virtual bool isDefaultExecStack() const
  { return true; }

  /// allocateCommonSymbols - allocate common symbols in the corresponding
  /// sections.
  /// Different concrete target backend may overlap this function.
  virtual bool allocateCommonSymbols(const MCLDInfo& pLDInfo, MCLinker& pLinker) const;

  /// isSymbolPreemtible - whether the symbol can be preemted by other
  /// link unit
  /// @ref Google gold linker, symtab.h:551
  bool isSymbolPreemptible(const ResolveInfo& pSym,
                           const MCLDInfo& pLDInfo,
                           const Output& pOutput) const;

  /// symbolNeedsDynRel - return whether the symbol needs a dynamic relocation
  /// @ref Google gold linker, symtab.h:645
  bool symbolNeedsDynRel(const ResolveInfo& pSym,
                         bool pSymHasPLT,
                         const MCLDInfo& pLDInfo,
                         const Output& pOutput,
                         bool isAbsReloc) const;

protected:
  uint64_t getSymbolSize(const LDSymbol& pSymbol) const;

  uint64_t getSymbolInfo(const LDSymbol& pSymbol) const;

  uint64_t getSymbolValue(const LDSymbol& pSymbol) const;

  uint64_t getSymbolShndx(const LDSymbol& pSymbol, const Layout& pLayout) const;

  /// getHashBucketCount - calculate hash bucket count.
  /// @ref Google gold linker, dynobj.cc:791
  static unsigned getHashBucketCount(unsigned pNumOfSymbols, bool pIsGNUStyle);

  /// isDynamicSymbol
  /// @ref Google gold linker: symtab.cc:311
  static bool isDynamicSymbol(const LDSymbol& pSymbol, const Output& pOutput);

  /// isOutputPIC - return whether the output is position-independent
  bool isOutputPIC(const Output& pOutput, const MCLDInfo& pInfo) const;

  /// isStaticLink - return whether we're doing static link
  bool isStaticLink(const Output& pOutput, const MCLDInfo& pInfo) const;

  /// symbolNeedsPLT - return whether the symbol needs a PLT entry
  /// @ref Google gold linker, symtab.h:596
  bool symbolNeedsPLT(const ResolveInfo& pSym,
                      const MCLDInfo& pLDInfo,
                      const Output& pOutput) const;

  /// symbolNeedsCopyReloc - return whether the symbol needs a copy relocation
  bool symbolNeedsCopyReloc(const Layout& pLayout,
                            const Relocation& pReloc,
                            const ResolveInfo& pSym,
                            const MCLDInfo& pLDInfo,
                            const Output& pOutput) const;

private:
  /// createProgramHdrs - base on output sections to create the program headers
  void createProgramHdrs(Output& pOutput,
                         const MCLDInfo& pInfo);

  /// setupProgramHdrs - set up the attributes of segments
  ///  (i.e., offset, addresses, file/mem size, flag,  and alignment)
  void setupProgramHdrs(const Output& pOutput, const MCLDInfo& pInfo);

  /// getSegmentFlag - give a section flag and return the corresponding segment
  /// flag
  inline uint32_t getSegmentFlag(const uint32_t pSectionFlag)
  {
    uint32_t flag = llvm::ELF::PF_R;
    if (0 != (pSectionFlag & llvm::ELF::SHF_WRITE))
      flag |= llvm::ELF::PF_W;
    if (0 != (pSectionFlag & llvm::ELF::SHF_EXECINSTR))
      flag |= llvm::ELF::PF_X;
    return flag;
  }

  /// createGNUStackInfo - create an output GNU stack section or segment if needed
  void createGNUStackInfo(const Output& pOutput,
                          const MCLDInfo& pInfo,
                          MCLinker& pLinker);

  /// preLayout - Backend can do any needed modification before layout
  void preLayout(const Output& pOutput,
                 const MCLDInfo& pInfo,
                 MCLinker& pLinker);

  /// postLayout -Backend can do any needed modification after layout
  void postLayout(const Output& pOutput,
                 const MCLDInfo& pInfo,
                 MCLinker& pLinker);

  /// preLayout - Backend can do any needed modification before layout
  virtual void doPreLayout(const Output& pOutput,
                         const MCLDInfo& pInfo,
                         MCLinker& pLinker) = 0;

  /// postLayout -Backend can do any needed modification after layout
  virtual void doPostLayout(const Output& pOutput,
                          const MCLDInfo& pInfo,
                          MCLinker& pLinker) = 0;

  /// postProcessing - Backend can do any needed modification in the final stage
  void postProcessing(const Output& pOutput,
                      const MCLDInfo& pInfo,
                      MCLinker& pLinker);

  /// dynamic - the dynamic section of the target machine.
  virtual ELFDynamic& dynamic() = 0;

  /// dynamic - the dynamic section of the target machine.
  virtual const ELFDynamic& dynamic() const = 0;

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
    SHO_EXCEPTION,           // .eh_frame_hdr, .eh_frame, .gcc_except_table
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

  typedef HashEntry<LDSymbol*, size_t, SymCompare> HashEntryType;
  typedef HashTable<HashEntryType, PtrHash, EntryFactory<HashEntryType> > HashTableType;

protected:
  // ----- readers and writers ----- //
  GNUArchiveReader* m_pArchiveReader;
  ELFObjectReader* m_pObjectReader;
  ELFDynObjReader* m_pDynObjReader;
  ELFObjectWriter* m_pObjectWriter;
  ELFDynObjWriter* m_pDynObjWriter;
  ELFExecWriter*   m_pExecWriter;

  // -----  file formats  ----- //
  ELFDynObjFileFormat* m_pDynObjFileFormat;
  ELFExecFileFormat* m_pExecFileFormat;

  // ELF segment factory
  ELFSegmentFactory m_ELFSegmentTable;

  // map the LDSymbol to its index in the output symbol table
  HashTableType* m_pSymIndexMap;

  // section .eh_frame_hdr
  EhFrameHdr* m_pEhFrameHdr;

  // -----  standard symbols  ----- //
  // section symbols
  LDSymbol* f_pPreInitArrayStart;
  LDSymbol* f_pPreInitArrayEnd;
  LDSymbol* f_pInitArrayStart;
  LDSymbol* f_pInitArrayEnd;
  LDSymbol* f_pFiniArrayStart;
  LDSymbol* f_pFiniArrayEnd;
  LDSymbol* f_pStack;

  // segment symbols
  LDSymbol* f_pExecutableStart;
  LDSymbol* f_pEText;
  LDSymbol* f_p_EText;
  LDSymbol* f_p__EText;
  LDSymbol* f_pEData;
  LDSymbol* f_p_EData;
  LDSymbol* f_pBSSStart;
  LDSymbol* f_pEnd;
  LDSymbol* f_p_End;
};

} // namespace of mcld

#endif

