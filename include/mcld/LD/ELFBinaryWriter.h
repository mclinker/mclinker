//===- ELFBinaryWriter.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_BINARY_WRITER_H
#define MCLD_ELF_BINARY_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/Support/system_error.h>
#include <mcld/LD/BinaryWriter.h>
#include <mcld/LD/ELFWriter.h>

namespace mcld {

class Module;
class MemoryArea;
class FragmentLinker;
class GNULDBackend;

/** \class ELFBinaryWriter
 *  \brief ELFBinaryWriter writes the target-independent parts of Binary files.
 *  ELFBinaryWriter reads a MCLDFile and writes into raw_ostream
 *
 */
class ELFBinaryWriter : public BinaryWriter, protected ELFWriter
{
public:
  ELFBinaryWriter(GNULDBackend& pBackend, FragmentLinker& pLinker);

  ~ELFBinaryWriter();

  llvm::error_code writeBinary(Module& pModule, MemoryArea& pOutput);

private:
  FragmentLinker& m_Linker;
};

} // namespace of mcld

#endif

