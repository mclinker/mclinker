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
#include <mcld/LD/DynObjWriter.h>
#include <mcld/LD/ELFWriter.h>

namespace mcld {

class Module;
class LinkerConfig;
class MemoryArea;
class GNULDBackend;

/** \class ELFDynObjWriter
 *  \brief ELFDynObjWriter writes the dynamic sections.
 */
class ELFDynObjWriter : public DynObjWriter, private ELFWriter
{
public:
  typedef ELFWriter::FileOffset FileOffset;

public:
  ELFDynObjWriter(GNULDBackend& pBackend,
                  const LinkerConfig& pConfig);

  ~ELFDynObjWriter();

  llvm::error_code writeDynObj(Module& pModule, MemoryArea& pOutput);

private:
  const LinkerConfig& m_Config;
};

} // namespace of mcld

#endif

