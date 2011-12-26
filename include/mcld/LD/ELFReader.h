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

#include <mcld/MC/MCLDInput.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/LD/ResolveInfoFactory.h>
#include <mcld/Support/rslinker/ELFObject.h>

#include <llvm/Support/ELF.h>

namespace mcld
{

/** \class ELFReader
 *  \brief ELFReader provides common functions for all kind of ELF readers.
 *  (ELFObjectReader, ELFDynObjReader)
 */
class ELFReader
{
public:
  Input::Type fileType(mcld::Input &pFile) const;

  bool isLittleEndian(mcld::Input &pFile) const;

  bool is64Bit(mcld::Input &pFile) const;

  std::auto_ptr<ELFObject<32> > createELFObject(mcld::Input &pFile) const;

  ResolveInfo::Binding getBindingResolveInfo(ELFSymbol<32>* sym, bool isDSO) const;

  ResolveInfo::Visibility getVisibilityResolveInfo(ELFSymbol<32>* sym) const;

  LDFileFormat::Kind getLDSectionKind(const ELFSectionHeader<32>& pHeader) const;

  LDFileFormat::Kind getLDSectionKind(const ELFSectionHeader<64>& pHeader) const;

  std::string getSymbolTypeName(unsigned int pType) const;
};

} // namespace of mcld

#endif

