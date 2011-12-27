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

/** \class ELFReader
 *  \brief ELFReader provides common functions for all kind of ELF readers.
 *  (ELFObjectReader, ELFDynObjReader)
 */
class ELFReader
{
public:
  bool isELF(Input& pInput) const;

  MCLDFile::Type fileType(Input &pInput) const;

  bool isLittleEndian(Input &pInput) const;

  unsigned int bitclass(Input &pInput) const;

  ELFObject<32>* createELF32Object(Input &pInput) const;

  ELFObject<64>* createELF64Object(Input &pInput) const;

  ResolveInfo::Binding getBindingResolveInfo(ELFSymbol<32>* sym, bool isDSO) const;

  ResolveInfo::Visibility getVisibilityResolveInfo(ELFSymbol<32>* sym) const;

  LDFileFormat::Kind getLDSectionKind(const ELFSectionHeader<32>& pHeader,
                                      const llvm::StringRef& pName) const;

  LDFileFormat::Kind getLDSectionKind(const ELFSectionHeader<64>& pHeader,
                                      const llvm::StringRef& pName) const;

  std::string getSymbolTypeName(unsigned int pType) const;
};

} // namespace of mcld

#endif

