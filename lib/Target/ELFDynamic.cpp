//===- ELFDynamic.cpp -------------    ------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/Host.h>
#include <llvm/Support/ErrorHandling.h>
#include <mcld/Target/ELFDynamic.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/LD/ELFFileFormat.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/Support/MemoryRegion.h>

using namespace mcld;
using namespace elf_dynamic;

//===----------------------------------------------------------------------===//
// elf_dynamic::EntryIF
EntryIF::EntryIF()
{
}

EntryIF::~EntryIF()
{
}

//===----------------------------------------------------------------------===//
// ELFDynamic
ELFDynamic::ELFDynamic(const GNULDBackend& pParent)
  : m_pEntryFactory(NULL), m_Idx(0) {
  if (32 == pParent.bitclass() && pParent.isLittleEndian()) {
    m_pEntryFactory = new Entry<32, true>();
  }
  // FIXME: support big-endian and 64-bit machine.
}


ELFDynamic::~ELFDynamic()
{
  if (NULL != m_pEntryFactory)
    delete m_pEntryFactory;

  EntryListType::iterator entry, entryEnd = m_EntryList.end();
  for (entry = m_EntryList.begin(); entry != entryEnd; ++entry) {
    if (NULL != *entry)
      delete (*entry);
  }

  entryEnd = m_NeedList.end();
  for (entry = m_NeedList.begin(); entry != entryEnd; ++entry) {
    if (NULL != *entry)
      delete (*entry);
  }
}

size_t ELFDynamic::size() const
{
  return (m_NeedList.size() + m_EntryList.size());
}

size_t ELFDynamic::numOfBytes() const
{
  return size()*entrySize();
}

size_t ELFDynamic::entrySize() const
{
  return m_pEntryFactory->size();
}

void ELFDynamic::reserveOne(uint64_t pTag)
{
  assert(NULL != m_pEntryFactory);
  m_EntryList.push_back(new elf_dynamic::Entry<32, true>());
}

void ELFDynamic::applyOne(uint64_t pTag, uint64_t pValue)
{
  assert(m_Idx < m_EntryList.size());
  m_EntryList[m_Idx]->setValue(pTag, pValue);
  ++m_Idx;
}

/// reserveEntries - reserve entries
void ELFDynamic::reserveEntries(const MCLDInfo& pLDInfo,
                                const ELFFileFormat& pFormat)
{
  if (pLDInfo.output().type() == Output::DynObj) {
    reserveOne(llvm::ELF::DT_SONAME); // DT_SONAME

    if (pLDInfo.options().Bsymbolic())
      reserveOne(llvm::ELF::DT_SYMBOLIC); // DT_SYMBOLIC
  }

  if (pFormat.hasInit())
    reserveOne(llvm::ELF::DT_INIT); // DT_INIT

  if (pFormat.hasFini())
    reserveOne(llvm::ELF::DT_FINI); // DT_FINI

  if (pFormat.hasInitArray()) {
    reserveOne(llvm::ELF::DT_INIT_ARRAY); // DT_INIT_ARRAY
    reserveOne(llvm::ELF::DT_INIT_ARRAYSZ); // DT_INIT_ARRAYSZ
  }

  if (pFormat.hasFiniArray()) {
    reserveOne(llvm::ELF::DT_FINI_ARRAY); // DT_FINI_ARRAY
    reserveOne(llvm::ELF::DT_FINI_ARRAYSZ); // DT_FINI_ARRAYSZ
  }

  if (pFormat.hasHashTab())
    reserveOne(llvm::ELF::DT_HASH); // DT_HASH

  if (pFormat.hasDynSymTab()) {
    reserveOne(llvm::ELF::DT_SYMTAB); // DT_SYMTAB
    reserveOne(llvm::ELF::DT_SYMENT); // DT_SYMENT
  }

  if (pFormat.hasDynStrTab()) {
    reserveOne(llvm::ELF::DT_STRTAB); // DT_STRTAB
    reserveOne(llvm::ELF::DT_STRSZ); // DT_STRSZ
  }

  reserveTargetEntries(pFormat); // DT_PLTGOT

  if (pFormat.hasRelPlt() || pFormat.hasRelaPlt())
    reserveOne(llvm::ELF::DT_PLTREL); // DT_PLTREL

  if (pFormat.hasPLT()) {
    reserveOne(llvm::ELF::DT_JMPREL); // DT_JMPREL
    reserveOne(llvm::ELF::DT_PLTRELSZ); // DT_PLTRELSZ
  }

  if (pFormat.hasRelDyn()) {
    reserveOne(llvm::ELF::DT_REL); // DT_REL
    reserveOne(llvm::ELF::DT_RELSZ); // DT_RELSZ
    reserveOne(llvm::ELF::DT_RELENT); // DT_RELENT
  }

  if (pFormat.hasRelaDyn()) {
    reserveOne(llvm::ELF::DT_RELA); // DT_RELA
    reserveOne(llvm::ELF::DT_RELASZ); // DT_RELASZ
    reserveOne(llvm::ELF::DT_RELAENT); // DT_RELAENT
  }

  if (pLDInfo.options().hasOrigin() ||
      pLDInfo.options().Bsymbolic() ||
      pLDInfo.options().hasNow()) {
    // TODO: add checks for DF_TEXTREL and DF_STATIC_TLS
    reserveOne(llvm::ELF::DT_FLAGS); // DT_FLAGS
  }

  if (pLDInfo.options().hasNow()          ||
      pLDInfo.options().hasLoadFltr()     ||
      pLDInfo.options().hasOrigin()       ||
      pLDInfo.options().hasInterPose()    ||
      pLDInfo.options().hasNoDefaultLib() ||
      pLDInfo.options().hasNoDump()       ||
      pLDInfo.options().Bgroup()          ||
      ((pLDInfo.output().type() == Output::DynObj) &&
       (pLDInfo.options().hasNoDelete()  ||
        pLDInfo.options().hasInitFirst() ||
        pLDInfo.options().hasNoDLOpen()))) {
    reserveOne(llvm::ELF::DT_FLAGS_1); // DT_FLAGS_1
  }

  reserveOne(llvm::ELF::DT_NULL); // for DT_NULL
}

/// applyEntries - apply entries
void ELFDynamic::applyEntries(const MCLDInfo& pInfo,
                              const ELFFileFormat& pFormat)
{
  if (pInfo.output().type() == Output::DynObj &&
      pInfo.options().Bsymbolic()) {
      applyOne(llvm::ELF::DT_SYMBOLIC, 0x0); // DT_SYMBOLIC
  }

  if (pFormat.hasInit())
    applyOne(llvm::ELF::DT_INIT, pFormat.getInit().addr()); // DT_INIT

  if (pFormat.hasFini())
    applyOne(llvm::ELF::DT_FINI, pFormat.getFini().addr()); // DT_FINI

  if (pFormat.hasInitArray()) {
    // DT_INIT_ARRAY
    applyOne(llvm::ELF::DT_INIT_ARRAY, pFormat.getInitArray().addr());

    // DT_INIT_ARRAYSZ
    applyOne(llvm::ELF::DT_INIT_ARRAYSZ, pFormat.getInitArray().size());
  }

  if (pFormat.hasFiniArray()) {
    // DT_FINI_ARRAY
    applyOne(llvm::ELF::DT_FINI_ARRAY, pFormat.getFiniArray().addr());

    // DT_FINI_ARRAYSZ
    applyOne(llvm::ELF::DT_FINI_ARRAYSZ, pFormat.getFiniArray().size());
  }

  if (pFormat.hasHashTab())
    applyOne(llvm::ELF::DT_HASH, pFormat.getHashTab().addr()); // DT_HASH

  if (pFormat.hasDynSymTab()) {
    applyOne(llvm::ELF::DT_SYMTAB, pFormat.getDynSymTab().addr()); // DT_SYMTAB
    applyOne(llvm::ELF::DT_SYMENT, symbolSize()); // DT_SYMENT
  }

  if (pFormat.hasDynStrTab()) {
    applyOne(llvm::ELF::DT_STRTAB, pFormat.getDynStrTab().addr()); // DT_STRTAB
    applyOne(llvm::ELF::DT_STRSZ, pFormat.getDynStrTab().size()); // DT_STRSZ
  }

  applyTargetEntries(pFormat); // DT_PLTGOT

  if (pFormat.hasRelPlt())
    applyOne(llvm::ELF::DT_PLTREL, llvm::ELF::DT_REL); // DT_PLTREL
  else if (pFormat.hasRelaPlt())
    applyOne(llvm::ELF::DT_PLTREL, llvm::ELF::DT_RELA); // DT_PLTREL

  if (pFormat.hasRelPlt()) {
    applyOne(llvm::ELF::DT_JMPREL, pFormat.getRelPlt().addr()); // DT_JMPREL
    applyOne(llvm::ELF::DT_PLTRELSZ, pFormat.getRelPlt().size()); // DT_PLTRELSZ
  }

  if (pFormat.hasRelDyn()) {
    applyOne(llvm::ELF::DT_REL, pFormat.getRelDyn().addr()); // DT_REL
    applyOne(llvm::ELF::DT_RELSZ, pFormat.getRelDyn().size()); // DT_RELSZ
    applyOne(llvm::ELF::DT_RELENT, m_pEntryFactory->relSize()); // DT_RELENT
  }

  if (pFormat.hasRelaDyn()) {
    applyOne(llvm::ELF::DT_RELA, pFormat.getRelaDyn().addr()); // DT_RELA
    applyOne(llvm::ELF::DT_RELASZ, pFormat.getRelaDyn().size()); // DT_RELASZ
    applyOne(llvm::ELF::DT_RELAENT, m_pEntryFactory->relaSize()); // DT_RELAENT
  }

  uint64_t dt_flags = 0x0;
  if (pInfo.options().hasOrigin())
    dt_flags |= llvm::ELF::DF_ORIGIN;
  if (pInfo.options().Bsymbolic())
    dt_flags |= llvm::ELF::DF_SYMBOLIC;
  if (pInfo.options().hasNow())
    dt_flags |= llvm::ELF::DF_BIND_NOW;
  // TODO: add checks for DF_TEXTREL and DF_STATIC_TLS
  if (0x0 != dt_flags) {
    applyOne(llvm::ELF::DT_FLAGS, dt_flags); // DT_FLAGS
  }

  uint64_t dt_flags_1 = 0x0;
  if (pInfo.options().hasNow())
    dt_flags_1 |= llvm::ELF::DF_1_NOW;
  if (pInfo.options().hasLoadFltr())
    dt_flags_1 |= llvm::ELF::DF_1_LOADFLTR;
  if (pInfo.options().hasOrigin())
    dt_flags_1 |= llvm::ELF::DF_1_ORIGIN;
  if (pInfo.options().hasInterPose())
    dt_flags_1 |= llvm::ELF::DF_1_INTERPOSE;
  if (pInfo.options().hasNoDefaultLib())
    dt_flags_1 |= llvm::ELF::DF_1_NODEFLIB;
  if (pInfo.options().hasNoDump())
    dt_flags_1 |= llvm::ELF::DF_1_NODUMP;
  if (pInfo.options().Bgroup())
    dt_flags_1 |= llvm::ELF::DF_1_GROUP;
  if (pInfo.output().type() == Output::DynObj) {
    if (pInfo.options().hasNoDelete())
      dt_flags_1 |= llvm::ELF::DF_1_NODELETE;
    if (pInfo.options().hasInitFirst())
      dt_flags_1 |= llvm::ELF::DF_1_INITFIRST;
    if (pInfo.options().hasNoDLOpen())
      dt_flags_1 |= llvm::ELF::DF_1_NOOPEN;
  }
  if (0x0 != dt_flags_1)
    applyOne(llvm::ELF::DT_FLAGS_1, dt_flags_1); // DT_FLAGS_1

  applyOne(llvm::ELF::DT_NULL, 0x0); // for DT_NULL
}

/// symbolSize
size_t ELFDynamic::symbolSize() const
{
  return m_pEntryFactory->symbolSize();
}

/// reserveNeedEntry - reserve on DT_NEED entry.
void ELFDynamic::reserveNeedEntry()
{
  m_NeedList.push_back(m_pEntryFactory->clone());
}

/// emit
void ELFDynamic::emit(const LDSection& pSection, MemoryRegion& pRegion) const
{
  if (pRegion.size() < pSection.size()) {
    llvm::report_fatal_error(llvm::Twine("the given memory is smaller") +
                             llvm::Twine(" than the section's demaind.\n"));
  }

  uint8_t* address = (uint8_t*)pRegion.start();
  EntryListType::const_iterator entry, entryEnd = m_NeedList.end();
  for (entry = m_NeedList.begin(); entry != entryEnd; ++entry)
    address += (*entry)->emit(address);

  entryEnd = m_EntryList.end();
  for (entry = m_EntryList.begin(); entry != entryEnd; ++entry)
    address += (*entry)->emit(address);
}

void ELFDynamic::applySoname(uint64_t pStrTabIdx)
{
  applyOne(llvm::ELF::DT_SONAME, pStrTabIdx); // DT_SONAME
}

