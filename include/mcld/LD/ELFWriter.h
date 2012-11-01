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
class GNULDBackend;
class Relocation;
class LDSection;
class SectionData;
class RelocData;
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
                                const Module& pModule,
                                MemoryArea& pOutput) const;

  virtual void writeELF64Header(const LinkerConfig& pConfig,
                                const Module& pModule,
                                MemoryArea& pOutput) const;

  virtual uint64_t getEntryPoint(const LinkerConfig& pConfig,
                                 const Module& pModule) const;

protected:
  void emitELF32SectionHeader(const Module& pModule,
                              const LinkerConfig& pConfig,
                              MemoryArea& pOutput) const;

  void emitELF64SectionHeader(const Module& pModule,
                              const LinkerConfig& pConfig,
                              MemoryArea& pOutput) const;

  void emitELF32ProgramHeader(MemoryArea& pOutput) const;

  void emitELF64ProgramHeader(MemoryArea& pOutput) const;

  // emitShStrTab - emit .shstrtab
  void emitELFShStrTab(const LDSection& pShStrTab, const Module& pModule,
                       MemoryArea& pOutput);

  void emitSectionData(const LDSection& pSection,
                       MemoryRegion& pRegion) const;

  void emitRelocation(const LinkerConfig& pConfig,
                      const LDSection& pSection,
                      MemoryRegion& pRegion) const;

  void emitRel(const LinkerConfig& pConfig,
               const RelocData& pRelocData,
               MemoryRegion& pRegion) const;

  void emitRela(const LinkerConfig& pConfig,
                const RelocData& pRelocData,
                MemoryRegion& pRegion) const;

private:
  // getSectEntrySize - compute ElfXX_Shdr::sh_entsize
  uint64_t getELF32SectEntrySize(const LDSection& pSection) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_entsize
  uint64_t getELF64SectEntrySize(const LDSection& pSection) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_link
  uint64_t getSectLink(const LDSection& pSection,
                       const LinkerConfig& pConfig) const;

  // getSectEntrySize - compute ElfXX_Shdr::sh_info
  uint64_t getSectInfo(const LDSection& pSection) const;

  uint64_t getELF32LastStartOffset(const Module& pModule) const;

  uint64_t getELF64LastStartOffset(const Module& pModule) const;

  void emitSectionData(const SectionData& pSD, MemoryRegion& pRegion) const;

protected:
  GNULDBackend& f_Backend;
};

} // namespace of mcld

#endif

