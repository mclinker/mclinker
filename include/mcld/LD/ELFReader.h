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
#include <mcld/MC/MCLDInput.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/ResolveInfoFactory.h>
#include <mcld/Support/rslinker/ELFObject.h>

namespace mcld
{

class GNULDBackend;
class MemoryRegion;

/** \class ELFReader
 *  \brief ELFReader provides common functions for all kind of ELF readers.
 *  (ELFObjectReader, ELFDynObjReader)
 */
class ELFReader
{
public:
  ELFReader(GNULDBackend& pBackend);

  virtual ~ELFReader() { }

  ELFObject<32>* createELF32Object(Input &pInput) const;

  ELFObject<64>* createELF64Object(Input &pInput) const;

  ResolveInfo::Binding getBindingResolveInfo(ELFSymbol<32>* sym, bool isDSO) const;

  ResolveInfo::Visibility getVisibilityResolveInfo(ELFSymbol<32>* sym) const;

  LDFileFormat::Kind getLDSectionKind(const ELFSectionHeader<32>& pHeader,
                                      const llvm::StringRef& pName) const;

  LDFileFormat::Kind getLDSectionKind(const ELFSectionHeader<64>& pHeader,
                                      const llvm::StringRef& pName) const;

  unsigned int bitclass(const uint8_t pEIdent[]) const;

  MCLDFile::Type fileType(const uint8_t pEIdent[]) const;

  bool isLittleEndian(const uint8_t pEIdent[]) const;

  bool isELF(const uint8_t pEIdent[]) const;

  bool readELF32Rel(const LDSection& pSection,
                    const MemoryRegion& pRegion,
                    LDContext& pContext,
                    MCLinker& pLinker);

  bool readELF32Rela(const LDSection& pSection,
                     const MemoryRegion& pRegion,
                     LDContext& pContext,
                     MCLinker& pLinker);

  bool readELF64Rel(const LDSection& pSection,
                    const MemoryRegion& pRegion,
                    LDContext& pContext,
                    MCLinker& pLinker);

  bool readELF64Rela(const LDSection& pSection,
                     const MemoryRegion& pRegion,
                     LDContext& pContext,
                     MCLinker& pLinker);

  std::string getSymbolTypeName(unsigned int pType) const;

  GNULDBackend& target()
  { return m_Backend; }

  const GNULDBackend& target() const
  { return m_Backend; }
  
private:
  GNULDBackend& m_Backend;
};

} // namespace of mcld

#endif

