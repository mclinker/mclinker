//===- ELFObjectWriter.h --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_OBJECT_WRITER_H
#define MCLD_ELF_OBJECT_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ObjectWriter.h>
#include <mcld/LD/ELFWriter.h>

#include <llvm/Support/system_error.h>

namespace mcld {

class Module;
class LinkerConfig;
class MemoryArea;
class GNULDBackend;

/** \class ELFObjectWriter
 *  \brief ELFObjectWriter writes the target-independent parts of object files.
 *  ELFObjectWriter reads a MCLDFile and writes into raw_ostream
 *
 */
class ELFObjectWriter : public ObjectWriter, protected ELFWriter
{
public:
  ELFObjectWriter(GNULDBackend& pBackend,
                  const LinkerConfig& pConfig);

  ~ELFObjectWriter();

  llvm::error_code writeObject(Module& pModule, MemoryArea& pOutput);

private:
  const LinkerConfig& m_Config;
};

} // namespace of mcld

#endif

