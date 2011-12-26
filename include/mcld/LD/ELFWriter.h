//===- ELFWriter.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_WRITER_H
#define MCLD_ELF_WRITER_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/Support/ELF.h>
#include <mcld/MC/MCLDOutput.h>

namespace mcld
{

class MCLDInfo;
class Layout;
class GNULDBackend;
class Relocation;

/** \class ELFWriter
 *  \brief ELFWriter provides basic functions to write ELF sections, symbols,
 *  and so on.
 */
class ELFWriter
{
public:
  typedef uint64_t FileOffset;

protected:
  ELFWriter(GNULDBackend& pBackend)
  : f_Backend(pBackend) {
  }

public:
  virtual ~ELFWriter() { }

  GNULDBackend& target()
  { return f_Backend; }

  const GNULDBackend& target() const
  { return f_Backend; }

  virtual void writeELF32Header(const MCLDInfo& pInfo,
                                const Layout& pLayout,
                                const GNULDBackend& pBackend,
                                Output& pOutput) const;

  virtual void writeELF64Header(const MCLDInfo& pInfo,
                                const Layout& pLayout,
                                const GNULDBackend& pBackend,
                                Output& pOutput) const;

  virtual uint64_t getEntry(const MCLDInfo& pInfo,
                            const Layout& pLayout,
                            const GNULDBackend& pBackend,
                            const Output& pOutput) const;

protected:
  FileOffset emitSectionHeader(Output& pOutput,
                               MCLinker& pLinker,
                               FileOffset pStartOffset) const;

  FileOffset emitSectionData(const LDSection& pSection,
                             MemoryRegion& pRegion) const;

  FileOffset emitRelocation(const LDSection& pSection,
                            MemoryRegion& pRegion) const;

  FileOffset emitRelEntry(const Relocation& pRelocation,
                          MemoryRegion& pRegion,
                          FileOffset pOffset,
                          bool pIsRela = false ) const;

protected:
  GNULDBackend& f_Backend;
};

} // namespace of mcld

#endif

