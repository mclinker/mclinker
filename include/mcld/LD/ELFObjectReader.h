//===- ELFObjectReader.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_OBJECT_READER_H
#define MCLD_ELF_OBJECT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/LD/ELFReader.h"
#include "mcld/LD/ObjectReader.h"
#include "llvm/Support/system_error.h"

using namespace ELF;

namespace mcld
{

class Input;
class MCLinker;

/** \lclass ELFObjectReader
 *  \brief ELFObjectReader reads target-independent parts of ELF object file
 */
class ELFObjectReader : public ObjectReader, protected ELFReader
{
public:
  ELFObjectReader(TargetLDBackend& pBackend, MCLinker& pLinker);

  ~ELFObjectReader();

  // -----  observers  ----- //
  LDReader::Endian endian(Input& pFile) const;

  bool isMyFormat(Input &pFile) const;

  // -----  readers  ----- //
  llvm::error_code readObject(Input& pFile);

  bool readSections(Input& pFile);

private:
  MCLinker& m_Linker;

};

} // namespace of mcld

#endif

