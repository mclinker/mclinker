//===- ELFDynObjWriter.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_EXECUTABLE_OBJECT_WRITER_H
#define MCLD_ELF_EXECUTABLE_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ExecWriter.h>
#include <mcld/LD/ELFWriter.h>

namespace mcld {

class Module;
class MemoryArea;
class GNULDBackend;
class FragmentLinker;

/** \class ELFDynObjWriter
 *  \brief ELFDynObjWriter writes the dynamic sections.
 */
class ELFExecWriter : public ExecWriter, private ELFWriter
{
public:
  typedef ELFWriter::FileOffset FileOffset;

public:
  ELFExecWriter(GNULDBackend& pBackend, FragmentLinker& pLinker);
  ~ELFExecWriter();

  llvm::error_code writeExecutable(Module& pModule, MemoryArea& pOutput);

private:
  FragmentLinker& m_Linker;
};

} // namespace of mcld

#endif

