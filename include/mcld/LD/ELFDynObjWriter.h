//===- ELFDynObjWriter.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_DYNAMIC_SHARED_OBJECT_WRITER_H
#define MCLD_ELF_DYNAMIC_SHARED_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/DenseMap.h>
#include <llvm/Support/ELF.h>
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ELFWriter.h>
#include <mcld/LD/LDContext.h>
#include <mcld/LD/LDSection.h>
#include <mcld/Support/MemoryArea.h>
#include <vector>
#include <utility>


namespace mcld {

class Module;
class GNULDBackend;
class FragmentLinker;
class MemoryArea;

/** \class ELFDynObjWriter
 *  \brief ELFDynObjWriter writes the dynamic sections.
 */
class ELFDynObjWriter : public DynObjWriter, private ELFWriter
{
public:
  typedef ELFWriter::FileOffset FileOffset;

public:
  ELFDynObjWriter(GNULDBackend& pBackend, FragmentLinker& pLinker);
  ~ELFDynObjWriter();

  llvm::error_code writeDynObj(Output& pOutput,
                               Module& pModule,
                               MemoryArea& pOut);

private:
  GNULDBackend& m_Backend;
  FragmentLinker& m_Linker;
};

} // namespace of mcld

#endif

