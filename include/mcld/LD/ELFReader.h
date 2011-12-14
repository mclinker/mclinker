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

  bool readDynSymbols(mcld::Input &pFile) const;
};

} // namespace of mcld

#endif

