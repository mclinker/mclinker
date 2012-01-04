//===- ELFDynObjReader.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_DYNAMIC_SHARED_OBJECT_READER_H
#define MCLD_ELF_DYNAMIC_SHARED_OBJECT_READER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/DynObjReader.h>
#include <llvm/Support/system_error.h>

namespace mcld
{

class Input;
class MCLinker;
class GNULDBackend;
class ELFReaderIF;

/** \class ELFDynObjReader
 *  \brief ELFDynObjReader reads ELF dynamic shared objects.
 *
 */
class ELFDynObjReader : public DynObjReader
{
public:
  ELFDynObjReader(GNULDBackend& pBackend, MCLinker& pLinker);
  ~ELFDynObjReader();

  // -----  observers  ----- //
  bool isMyFormat(Input &pFile) const;

  // -----  readers  ----- //
  bool readDSO(Input& pFile);

  bool readSymbols(Input& pInput);

private:
  ELFReaderIF *m_pELFReader;
  MCLinker& m_Linker;
};

} // namespace of mcld

#endif

