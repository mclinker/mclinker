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
#include <vector>
#include <utility>

namespace mcld
{

class MCLinker;
class TargetLDBackend;

/** \class ELFDynObjWriter
 *  \brief ELFDynObjWriter writes the dynamic sections.
 */
class ELFDynObjWriter : public DynObjWriter, private ELFWriter
{
public:
  typedef std::vector< std::pair<const char *, uint32_t> > StrTab;

public:
  ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker);
  bool WriteObject();
  bool WriteELFHeader();
  ~ELFDynObjWriter();

private:
  const static uint32_t DefaultEABIVersion = 0x05000000;

  uint32_t WriteDynSymTab(uint32_t file_offset);

  uint32_t WriteDynStrTab(uint32_t file_offset);

  uint32_t WriteSymbolEntry(uint32_t file_offset,
                        uint32_t name, uint8_t info,
                        uint32_t value, uint32_t size,
                        uint8_t other, uint32_t shndx);

  // .dynstr
  StrTab dynstrTab;

  // .shstrtab
  StrTab shstrTab;

  MCLinker& m_Linker;
};

} // namespace of mcld

#endif

