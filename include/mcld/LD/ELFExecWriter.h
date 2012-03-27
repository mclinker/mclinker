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
#include <llvm/ADT/DenseMap.h>
#include <llvm/Support/ELF.h>
#include <mcld/LD/ExecWriter.h>
#include <mcld/LD/ELFWriter.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSection.h>
#include <mcld/Support/MemoryArea.h>
#include <vector>
#include <utility>


namespace mcld
{

class GNULDBackend;
class MCLinker;

/** \class ELFDynObjWriter
 *  \brief ELFDynObjWriter writes the dynamic sections.
 */
class ELFExecWriter : public ExecWriter, private ELFWriter
{
public:
  typedef ELFWriter::FileOffset FileOffset;

public:
  ELFExecWriter(GNULDBackend& pBackend, MCLinker& pLinker);
  ~ELFExecWriter();

  llvm::error_code writeExecutable(Output& pOutput);

private:
  GNULDBackend& m_Backend;
  MCLinker& m_Linker;
};

} // namespace of mcld

#endif

