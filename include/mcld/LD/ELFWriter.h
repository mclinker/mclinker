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

namespace mcld {

class Module;
class FragmentLinker;
class LinkerConfig;
class Layout;
class GNULDBackend;
class Relocation;
class LDSection;
class SectionData;
class Output;
class MemoryRegion;
class MemoryArea;

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

  virtual void writeELF32Header(const LinkerConfig& pConfig,
                                const GNULDBackend& pBackend,
                                const Module& pModule,
                                const Layout& pLayout,
                                Output& pOutput,
                                MemoryArea& pOut) const;

  virtual void writeELF64Header(const LinkerConfig& pConfig,
                                const GNULDBackend& pBackend,
                                const Module& pModule,
                                const Layout& pLayout,
                                Output& pOutput,
                                MemoryArea& pOut) const;

  virtual uint64_t getEntryPoint(const LinkerConfig& pConfig,
                                 const GNULDBackend& pBackend,
                                 const Module& pModule,
                                 const Layout& pLayout) const;

protected:
  void emitELF32SectionHeader(Output& pOutput,
                              const LinkerConfig& pConfig,
                              FragmentLinker& pLinker,
                              MemoryArea& pOut) const;

  void emitELF64SectionHeader(Output& pOutput,
                              const LinkerConfig& pConfig,
                              FragmentLinker& pLinker,
                              MemoryArea& pOut) const;

  void emitELF32ProgramHeader(const GNULDBackend& pBackend,
                              MemoryArea& pOut) const;

  void emitELF64ProgramHeader(const GNULDBackend& pBackend,
                              MemoryArea& pOut) const;

  // emitShStrTab - emit .shstrtab
  void emitELF32ShStrTab(Output& pOutput,
                         FragmentLinker& pLinker,
                         MemoryArea& pOut) const;

  void emitELF64ShStrTab(Output& pOutput,
                         FragmentLinker& pLinker,
                         MemoryArea& pOut) const;

  void emitSectionData(const Layout& pLayout,
                       const LDSection& pSection,
                       MemoryRegion& pRegion) const;

  void emitRelocation(const Layout& pLayout,
                      const LinkerConfig& pConfig,
                      const LDSection& pSection,
                      MemoryRegion& pRegion) const;

  void emitRel(const Layout& pLayout,
               const LinkerConfig& pConfig,
               const SectionData& pSectionData,
               MemoryRegion& pRegion) const;

  void emitRela(const Layout& pLayout,
                const LinkerConfig& pConfig,
                const SectionData& pSectionData,
                MemoryRegion& pRegion) const;

private:
  // getSectEntrySize - compute ElfXX_Shdr::sh_entsize
  uint64_t getELF32SectEntrySize(const LDSection& pSection) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_entsize
  uint64_t getELF64SectEntrySize(const LDSection& pSection) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_link
  uint64_t getSectLink(const LDSection& pSection,
                       const Output& pOutput,
                       const LinkerConfig& pConfig) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_info
  uint64_t getSectInfo(const LDSection& pSection) const;

  uint64_t getELF32LastStartOffset(const Output& pOutput) const;

  uint64_t getELF64LastStartOffset(const Output& pOutput) const;

protected:
  GNULDBackend& f_Backend;
};

} // namespace of mcld

#endif

