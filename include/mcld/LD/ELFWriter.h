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
class LDSection;

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

  virtual FileOffset writeELF32Header(const MCLDInfo& pInfo,
                                      const Layout& pLayout,
                                      const GNULDBackend& pBackend,
                                      Output& pOutput,
                                      FileOffset pShdrOffset) const;

  virtual FileOffset writeELF64Header(const MCLDInfo& pInfo,
                                      const Layout& pLayout,
                                      const GNULDBackend& pBackend,
                                      Output& pOutput,
                                      FileOffset pShdrOffset) const;

  virtual uint64_t getEntry(const MCLDInfo& pInfo,
                            const Layout& pLayout,
                            const GNULDBackend& pBackend,
                            const Output& pOutput) const;

protected:
  FileOffset emitELF32SectionHeader(Output& pOutput,
                                    MCLinker& pLinker,
                                    FileOffset pStartOffset) const;

  FileOffset emitELF64SectionHeader(Output& pOutput,
                                    MCLinker& pLinker,
                                    FileOffset pStartOffset) const;

  // emitShStrTab - emit .shstrtab
  FileOffset emitShStrTab(Output& pOutput,
                          LDSection& pShStrSect,
                          FileOffset pStartOffset) const;

  FileOffset emitSectionData(const LDSection& pSection,
                             MemoryRegion& pRegion) const;

  FileOffset emitRelocation(const LDSection& pSection,
                            MemoryRegion& pRegion) const;

  FileOffset emitRelEntry(const Relocation& pRelocation,
                          MemoryRegion& pRegion,
                          FileOffset pOffset,
                          bool pIsRela = false ) const;

private:
  // getSectEntrySize - compute ElfXX_Shdr::sh_entsize
  uint64_t getSectEntrySize(const LDSection& pSection) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_link
  uint64_t getSectLink(const LDSection& pSection, const Output& pOutput) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_info
  uint64_t getSectInfo(const LDSection& pSection, const Output& pOutput) const;

protected:
  GNULDBackend& f_Backend;
};

} // namespace of mcld

#endif

