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

#include <llvm/Support/system_error.h>

namespace mcld {

class Module;
class LinkerConfig;
class GNULDBackend;
class FragmentLinker;
class Relocation;
class LDSection;
class SectionData;
class RelocData;
class Output;
class MemoryRegion;
class MemoryArea;

/** \class ELFObjectWriter
 *  \brief ELFObjectWriter writes the target-independent parts of object files.
 *  ELFObjectWriter reads a MCLDFile and writes into raw_ostream
 *
 */
class ELFObjectWriter : public ObjectWriter
{
public:
  typedef uint64_t FileOffset;

public:
  ELFObjectWriter(GNULDBackend& pBackend,
                  const LinkerConfig& pConfig);

  ~ELFObjectWriter();

  llvm::error_code writeObject(Module& pModule, MemoryArea& pOutput);

private:
  void writeSection(MemoryArea& pOutput, LDSection *section);

  GNULDBackend&       target()        { return m_Backend; }

  const GNULDBackend& target() const  { return m_Backend; }

  virtual void writeELF32Header(const LinkerConfig& pConfig,
                                const Module& pModule,
                                MemoryArea& pOutput) const;

  virtual void writeELF64Header(const LinkerConfig& pConfig,
                                const Module& pModule,
                                MemoryArea& pOutput) const;

  virtual uint64_t getEntryPoint(const LinkerConfig& pConfig,
                                 const Module& pModule) const;

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

  template<unsigned int SIZE>
  void emitRel(const LinkerConfig& pConfig,
               const RelocData& pRelocData,
               MemoryRegion& pRegion) const;

  template<unsigned int SIZE>
  void emitRela(const LinkerConfig& pConfig,
                const RelocData& pRelocData,
                MemoryRegion& pRegion) const;

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

private:
  GNULDBackend& m_Backend;

  const LinkerConfig& m_Config;
};

template<>
void ELFObjectWriter::emitRel<32>(const LinkerConfig& pConfig,
                                  const RelocData& pRelocData,
                                  MemoryRegion& pRegion) const;

template<>
void ELFObjectWriter::emitRela<32>(const LinkerConfig& pConfig,
                                   const RelocData& pRelocData,
                                   MemoryRegion& pRegion) const;

template<>
void ELFObjectWriter::emitRel<64>(const LinkerConfig& pConfig,
                                  const RelocData& pRelocData,
                                  MemoryRegion& pRegion) const;

template<>
void ELFObjectWriter::emitRela<64>(const LinkerConfig& pConfig,
                                   const RelocData& pRelocData,
                                   MemoryRegion& pRegion) const;

} // namespace of mcld

#endif

