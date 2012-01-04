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

#include <mcld/LD/ObjectReader.h>
#include <llvm/Support/system_error.h>

namespace mcld
{

class Input;
class MCLinker;
class GNULDBackend;
class ELFReaderIF;

/** \lclass ELFObjectReader
 *  \brief ELFObjectReader reads target-independent parts of ELF object file
 */
class ELFObjectReader : public ObjectReader
{
public:
  ELFObjectReader(GNULDBackend& pBackend, MCLinker& pLinker);

  ~ELFObjectReader();

  // -----  observers  ----- //
  bool isMyFormat(Input &pFile) const;

  // -----  readers  ----- //
  bool readObject(Input& pFile);

  virtual bool readSections(Input& pFile);

  virtual bool readSymbols(Input& pFile);

  /// readRelocations - read relocation sections
  ///
  /// This function should be called after symbol resolution.
  virtual bool readRelocations(Input& pFile);

private:
  ELFReaderIF* m_pELFReader;
  MCLinker& m_Linker;
};

} // namespace of mcld

#endif

