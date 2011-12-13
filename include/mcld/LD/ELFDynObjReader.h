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
#include "mcld/LD/ELFReader.h"
#include "mcld/LD/DynObjReader.h"
#include "llvm/Support/system_error.h"

namespace mcld
{

class Input;
class MCLinker;

/** \class ELFDynObjReader
 *  \brief ELFDynObjReader reads ELF dynamic shared objects.
 *
 */
class ELFDynObjReader : public DynObjReader, protected ELFReader
{
public:
  ELFDynObjReader(TargetLDBackend& pBackend, MCLinker& pLinker);
  ~ELFDynObjReader();

  // -----  observers  ----- //
  LDReader::Endian endian(Input& pFile) const;

  bool isMyFormat(Input &pFile) const;

  // -----  readers  ----- //
  llvm::error_code readDSO(Input& pFile);

private:
  MCLinker& m_Linker;
};

} // namespace of mcld

#endif

