//===- ELFReader.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_READER_INTERFACE_H
#define MCLD_ELF_READER_INTERFACE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/StringRef.h>
#include <llvm/Support/ELF.h>
#include <llvm/Support/Host.h>

#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/Fragment.h>
#include <mcld/LD/FillFragment.h>
#include <mcld/LD/AlignFragment.h>
#include <mcld/LD/RegionFragment.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/LDContext.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>

namespace mcld
{

/** \class ELFReaderIF
 *  \brief ELFReaderIF provides common interface for all kind of ELF readers.
 */
class ELFReaderIF
{
public:
  ELFReaderIF(GNULDBackend& pBackend)
    : m_Backend(pBackend)
  { }

  virtual ~ELFReaderIF() { }

  /// ELFHeaderSize - return the size of the ELFHeader
  virtual size_t getELFHeaderSize() const = 0;

  /// isELF - is this a ELF file
  virtual bool isELF(void* pELFHeader) const = 0;

  /// isMyEndian - is this ELF file in the same endian to me?
  virtual bool isMyEndian(void* pELFHeader) const = 0;

  /// isMyMachine - is this ELF file generated for the same machine.
  virtual bool isMyMachine(void* pELFHeader) const = 0;

  /// fileType - the file type of this file
  virtual MCLDFile::Type fileType(void* pELFHeader) const = 0;

  /// target - the target backend
  GNULDBackend& target()
  { return m_Backend; }

  /// target - the target backend
  const GNULDBackend& target() const
  { return m_Backend; }

  /// readSectionHeaders - read ELF section header table and create LDSections
  virtual bool readSectionHeaders(Input& pInput,
                                  MCLinker& pLinker,
                                  void* pELFHeader) const = 0;

  /// readRegularSection - read a regular section and create fragments.
  virtual bool readRegularSection(Input& pInput,
                                  MCLinker& pLinker,
                                  LDSection& pSectHdr) const = 0;

  /// readRegularSection - read a target section and create fragments.
  virtual bool readTargetSection(Input& pInput,
                                 MCLinker& pLinker,
                                 LDSection& pSectHdr) = 0;

  /// readSymbols - read ELF symbols and create LDSymbol
  virtual bool readSymbols(Input& pInput,
                           MCLinker& pLinker,
                           const MemoryRegion& pRegion,
                           const char* StrTab) const = 0;

  /// readSymbol - read a symbol from the given Input and index in symtab
  /// This is used to get the signature of a group section.
  virtual ResolveInfo* readSymbol(Input& pInput,
                                  LDSection& pSymTab,
                                  MCLDInfo& pLDInfo,
                                  uint32_t pSymIdx) const = 0;

  /// readRela - read ELF rela and create Relocation
  virtual bool readRela(Input& pInput,
                        MCLinker& pLinker,
                        LDSection& pSection,
                        const MemoryRegion& pRegion) const = 0;

  /// readRel - read ELF rel and create Relocation
  virtual bool readRel(Input& pInput,
                       MCLinker& pLinker,
                       LDSection& pSection,
                       const MemoryRegion& pRegion) const = 0;

  bool readEhFrame(Input& pInput,
                   MCLinker& pLinker,
                   LDSection& pSection) const;

  /// readDynamic - read ELF .dynamic in input dynobj
  virtual bool readDynamic(Input& pInput) const = 0;

protected:
  /// LinkInfo - some section needs sh_link and sh_info, remember them.
  struct LinkInfo {
    LDSection* section;
    uint32_t sh_link;
    uint32_t sh_info;
  };

  typedef std::vector<LinkInfo> LinkInfoList;

protected:
  LDFileFormat::Kind getLDSectionKind(uint32_t pType, const char* pName) const;

  ResolveInfo::Type getSymType(uint8_t pInfo, uint16_t pShndx) const;

  ResolveInfo::Desc getSymDesc(uint16_t pShndx, const Input& pInput) const;

  ResolveInfo::Binding getSymBinding(uint8_t pBinding,
                                     uint16_t pShndx,
                                     uint8_t pVisibility) const;

  uint64_t getSymValue(uint64_t pValue,
                       uint16_t pShndx,
                       const Input& pInput) const;

  FragmentRef* getSymFragmentRef(Input& pInput,
                                 MCLinker& pLinker,
                                 uint16_t pShndx,
                                 uint32_t pOffset) const;

  ResolveInfo::Visibility getSymVisibility(uint8_t pVis) const;

private:
  GNULDBackend& m_Backend;
};

/** \class ELFReader
 *  \brief ELFReader is a template scaffolding for partial specification.
 */
template<size_t BIT, bool LITTLEENDIAN>
class ELFReader
{ };

/** \class ELFReader<32, true>
 *  \brief ELFReader<32, true> is a 32-bit, little endian ELFReader.
 */
template<>
class ELFReader<32, true> : public ELFReaderIF
{
public:
  typedef llvm::ELF::Elf32_Ehdr ELFHeader;
  typedef llvm::ELF::Elf32_Shdr SectionHeader;
  typedef llvm::ELF::Elf32_Sym  Symbol;
  typedef llvm::ELF::Elf32_Rel  Rel;
  typedef llvm::ELF::Elf32_Rela Rela;

public:
  inline ELFReader(GNULDBackend& pBackend);

  inline ~ELFReader();

  /// ELFHeaderSize - return the size of the ELFHeader
  inline size_t getELFHeaderSize() const
  { return sizeof(ELFHeader); }

  /// isELF - is this a ELF file
  inline bool isELF(void* pELFHeader) const;

  /// isMyEndian - is this ELF file in the same endian to me?
  inline bool isMyEndian(void* pELFHeader) const;

  /// isMyMachine - is this ELF file generated for the same machine.
  inline bool isMyMachine(void* pELFHeader) const;

  /// fileType - the file type of this file
  inline MCLDFile::Type fileType(void* pELFHeader) const;

  /// readSectionHeaders - read ELF section header table and create LDSections
  inline bool readSectionHeaders(Input& pInput,
                          MCLinker& pLinker,
                          void* pELFHeader) const;

  /// readRegularSection - read a regular section and create fragments.
  inline bool readRegularSection(Input& pInput,
                                 MCLinker& pLinker,
                                 LDSection& pInputSectHdr) const;

  /// readRegularSection - read a target section and create fragments.
  inline bool readTargetSection(Input& pInput,
                                MCLinker& pLinker,
                                LDSection& pInputSectHdr);

  /// readSymbols - read ELF symbols and create LDSymbol
  inline bool readSymbols(Input& pInput,
                          MCLinker& pLinker,
                          const MemoryRegion& pRegion,
                          const char* StrTab) const;

  /// readSymbol - read a symbol from the given Input and index in symtab
  /// This is used to get the signature of a group section.
  inline ResolveInfo* readSymbol(Input& pInput,
                                 LDSection& pSymTab,
                                 MCLDInfo& pLDInfo,
                                 uint32_t pSymIdx) const;

  /// readRela - read ELF rela and create Relocation
  inline bool readRela(Input& pInput,
                       MCLinker& pLinker,
                       LDSection& pSection,
                       const MemoryRegion& pRegion) const;

  /// readRel - read ELF rel and create Relocation
  inline bool readRel(Input& pInput,
                      MCLinker& pLinker,
                      LDSection& pSection,
                      const MemoryRegion& pRegion) const;

  /// readDynamic - read ELF .dynamic in input dynobj
  inline bool readDynamic(Input& pInput) const;
};

#include "ELFReader.tcc"

} // namespace of mcld

#endif

