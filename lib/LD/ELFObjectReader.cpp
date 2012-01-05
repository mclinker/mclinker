//===- ELFObjectReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ELF.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/ErrorHandling.h>
#include <mcld/LD/ELFObjectReader.h>
#include <mcld/LD/ELFReader.h>
#include <mcld/MC/MCLDInput.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/MC/MCRegionFragment.h>
#include <mcld/Target/GNULDBackend.h>

#include <string>
#include <cassert>

using namespace llvm;
using namespace mcld;

//==========================
// ELFObjectReader
/// constructor
ELFObjectReader::ELFObjectReader(GNULDBackend& pBackend, MCLinker& pLinker)
  : ObjectReader(),
    m_pELFReader(0),
    m_Linker(pLinker)
{
  if (32 == pBackend.bitclass() && pBackend.isLittleEndian()) {
    m_pELFReader = new ELFReader<32, true>(pBackend);
  }
}

/// destructor
ELFObjectReader::~ELFObjectReader()
{
  delete m_pELFReader;
}

/// isMyFormat
bool ELFObjectReader::isMyFormat(Input &pInput) const
{
  assert(pInput.hasMemArea());

  // Don't warning about the frequently requests.
  // MemoryArea has a list of cache to handle this.
  size_t hdr_size = m_pELFReader->getELFHeaderSize();
  MemoryRegion* region = pInput.memArea()->request(0, hdr_size);

  uint8_t* ELF_hdr = region->start();
  if (!m_pELFReader->isELF(ELF_hdr))
    return false;
  if (!m_pELFReader->isMyEndian(ELF_hdr))
    return false;
  if (!m_pELFReader->isMyMachine(ELF_hdr))
    return false;
  return (MCLDFile::Object == m_pELFReader->fileType(ELF_hdr));
}

/// readObject - read section header and create LDSections.
bool ELFObjectReader::readObject(Input& pInput)
{
  assert(pInput.hasMemArea());

  size_t hdr_size = m_pELFReader->getELFHeaderSize();
  MemoryRegion* region = pInput.memArea()->request(0, hdr_size);
  uint8_t* ELF_hdr = region->start();

  return m_pELFReader->readSectionHeaders(pInput, m_Linker, ELF_hdr);
}

/// readSections - read all regular sections.
bool ELFObjectReader::readSections(Input& pInput)
{
  // handle sections
  LDContext::sect_iterator section, sectEnd = pInput.context()->sectEnd();
  for (section = pInput.context()->sectBegin(); section != sectEnd; ++section) {
    switch((*section)->kind()) {
      // FIXME: support Debug Kind
      case LDFileFormat::Debug:
      /** Fall through **/
      case LDFileFormat::Regular:
      case LDFileFormat::Note:
      case LDFileFormat::Target:
      case LDFileFormat::MetaData: {
        MemoryRegion* region = pInput.memArea()->request((*section)->offset(),
                                                         (*section)->size());
        llvm::MCSectionData& sect_data = m_Linker.getOrCreateSectData(**section);
        new MCRegionFragment(*region, &sect_data);
        break;
      }
      case LDFileFormat::BSS: {
        LDSection& output_bss = m_Linker.getOrCreateOutputSectHdr(
                                               ".bss",
                                               LDFileFormat::BSS,
                                               llvm::ELF::SHT_NOBITS,
                                               llvm::ELF::SHF_ALLOC | llvm::ELF::SHF_WRITE);

        llvm::MCSectionData& sect_data = m_Linker.getOrCreateSectData(**section);
                             /*  value, valsize, size,               SD */
        new llvm::MCFillFragment(0x0,   1,       (*section)->size(), &sect_data);
        output_bss.setSize(output_bss.size() + (*section)->size());
        break;
      }
      // ignore
      case LDFileFormat::Null:
      case LDFileFormat::NamePool:
      case LDFileFormat::Relocation:
        continue;
    }
  } // end of for all sections

  return true;
}

/// readSymbols - read symbols into MCLinker from the input relocatable object.
bool ELFObjectReader::readSymbols(Input& pInput)
{
  assert(pInput.hasMemArea());

  LDSection* symtab_shdr = pInput.context()->getSection(".symtab");
  LDSection* strtab_shdr = symtab_shdr->getLinkInfo();
  if (NULL == symtab_shdr || NULL == strtab_shdr)
    return false;

  MemoryRegion* symtab_region = pInput.memArea()->request(symtab_shdr->offset(),
                                                          symtab_shdr->size());
  MemoryRegion* strtab_region = pInput.memArea()->request(strtab_shdr->offset(),
                                                          strtab_shdr->size());
  char* strtab = reinterpret_cast<char*>(strtab_region->start());
  return m_pELFReader->readSymbols(pInput, m_Linker, *symtab_region, strtab);
}

bool ELFObjectReader::readRelocations(Input& pInput)
{
  assert(pInput.hasMemArea());

  MemoryArea* mem = pInput.memArea();
  LDContext::sect_iterator section, sectEnd = pInput.context()->sectEnd();
  for (section = pInput.context()->sectBegin(); section != sectEnd; ++section) {
    if ((*section)->type() == llvm::ELF::SHT_RELA &&
        (*section)->kind() == LDFileFormat::Relocation) {
      MemoryRegion* region = mem->request((*section)->offset(), (*section)->size());
      if (!m_pELFReader->readRela(pInput, m_Linker, **section, *region))
        return false;
    }
    else if ((*section)->type() == llvm::ELF::SHT_REL &&
             (*section)->kind() == LDFileFormat::Relocation) {
      MemoryRegion* region = mem->request((*section)->offset(), (*section)->size());
      if (!m_pELFReader->readRel(pInput, m_Linker, **section, *region))
        return false;
    }
  }
  return true;
}

