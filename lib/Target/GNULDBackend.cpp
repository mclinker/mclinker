//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ELF.h>
#include <mcld/ADT/SizeTraits.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDInputTree.h>
#include <mcld/MC/SymbolCategory.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/Layout.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <string>
#include <cstring>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GNULDBackend
GNULDBackend::GNULDBackend()
  : m_pArchiveReader(0),
    m_pObjectReader(0),
    m_pDynObjReader(0),
    m_pObjectWriter(0),
    m_pDynObjWriter(0),
    m_pDynObjFileFormat(0),
    m_pExecFileFormat(0),
    m_ELFSegmentTable(9)// magic number
{
  m_pSymIndexMap = new HashTableType(1024);
}

GNULDBackend::~GNULDBackend()
{
  if (m_pArchiveReader)
    delete m_pArchiveReader;
  if (m_pObjectReader)
    delete m_pObjectReader;
  if (m_pDynObjReader)
    delete m_pDynObjReader;
  if (m_pObjectWriter)
    delete m_pObjectWriter;
  if (m_pDynObjWriter)
    delete m_pDynObjWriter;
  if (m_pDynObjFileFormat)
    delete m_pDynObjFileFormat;
  if (m_pExecFileFormat)
    delete m_pExecFileFormat;
  if(m_pSymIndexMap)
    delete m_pSymIndexMap;
}

size_t GNULDBackend::sectionStartOffset() const
{
  // FIXME: use fixed offset, we need 10 segments by default
  return sizeof(llvm::ELF::Elf64_Ehdr)+10*sizeof(llvm::ELF::Elf64_Phdr);
}

bool GNULDBackend::initArchiveReader(MCLinker&, MCLDInfo &pInfo)
{
  if (0 == m_pArchiveReader)
  {
    LDReader::Endian isLittleEndian = LDReader::LittleEndian;
    m_pArchiveReader = new GNUArchiveReader(pInfo, isLittleEndian);
  }
  return true;
}

bool GNULDBackend::initObjectReader(MCLinker& pLinker)
{
  if (0 == m_pObjectReader)
    m_pObjectReader = new ELFObjectReader(*this, pLinker);
  return true;
}

bool GNULDBackend::initDynObjReader(MCLinker& pLinker)
{
  if (0 == m_pDynObjReader)
    m_pDynObjReader = new ELFDynObjReader(*this, pLinker);
  return true;
}

bool GNULDBackend::initObjectWriter(MCLinker&)
{
  // TODO
  return true;
}

bool GNULDBackend::initDynObjWriter(MCLinker& pLinker)
{
  if (0 == m_pDynObjWriter)
    m_pDynObjWriter = new ELFDynObjWriter(*this, pLinker);
  return true;
}

bool GNULDBackend::initExecSections(MCLinker& pMCLinker)
{
  if (0 == m_pExecFileFormat)
    m_pExecFileFormat = new ELFExecFileFormat(*this);

  // initialize standard sections
  m_pExecFileFormat->initStdSections(pMCLinker);
  return true;
}

bool GNULDBackend::initDynObjSections(MCLinker& pMCLinker)
{
  if (0 == m_pDynObjFileFormat)
    m_pDynObjFileFormat = new ELFDynObjFileFormat(*this);

  // initialize standard sections
  m_pDynObjFileFormat->initStdSections(pMCLinker);
  return true;
}

bool GNULDBackend::initStandardSymbols(MCLinker& pLinker)
{
  return true;
}

GNUArchiveReader *GNULDBackend::getArchiveReader()
{
  assert(0 != m_pArchiveReader);
  return m_pArchiveReader;
}

GNUArchiveReader *GNULDBackend::getArchiveReader() const
{
  assert(0 != m_pArchiveReader);
  return m_pArchiveReader;
}

ELFObjectReader *GNULDBackend::getObjectReader()
{
  assert(0 != m_pObjectReader);
  return m_pObjectReader;
}

ELFObjectReader *GNULDBackend::getObjectReader() const
{
  assert(0 != m_pObjectReader);
  return m_pObjectReader;
}

ELFDynObjReader *GNULDBackend::getDynObjReader()
{
  assert(0 != m_pDynObjReader);
  return m_pDynObjReader;
}

ELFDynObjReader *GNULDBackend::getDynObjReader() const
{
  assert(0 != m_pDynObjReader);
  return m_pDynObjReader;
}

ELFObjectWriter *GNULDBackend::getObjectWriter()
{
  // TODO
  return NULL;
}

ELFObjectWriter *GNULDBackend::getObjectWriter() const
{
  // TODO
  return NULL;
}

ELFDynObjWriter *GNULDBackend::getDynObjWriter()
{
  assert(0 != m_pDynObjWriter);
  return m_pDynObjWriter;
}

ELFDynObjWriter *GNULDBackend::getDynObjWriter() const
{
  assert(0 != m_pDynObjWriter);
  return m_pDynObjWriter;
}

ELFDynObjFileFormat* GNULDBackend::getDynObjFileFormat()
{
  assert(0 != m_pDynObjFileFormat);
  return m_pDynObjFileFormat;
}

ELFDynObjFileFormat* GNULDBackend::getDynObjFileFormat() const
{
  assert(0 != m_pDynObjFileFormat);
  return m_pDynObjFileFormat;
}

ELFExecFileFormat* GNULDBackend::getExecFileFormat()
{
  assert(0 != m_pExecFileFormat);
  return m_pExecFileFormat;
}

ELFExecFileFormat* GNULDBackend::getExecFileFormat() const
{
  assert(0 != m_pExecFileFormat);
  return m_pExecFileFormat;
}

/// sizeNamePools - compute the size of regular name pools
/// In ELF executable files, regular name pools are .symtab, .strtab,
/// .dynsym, .dynstr, and .hash
void
GNULDBackend::sizeNamePools(const Output& pOutput,
                            const SymbolCategory& pSymbols,
                            const MCLDInfo& pLDInfo)
{
  // size of string tables starts from 1 to hold the null character in their
  // first byte
  size_t symtab = 1;
  size_t dynsym = 1;
  // number of entries in symbol tables starts from 1 to hold the special entry
  // at index 0 (STN_UNDEF). See ELF Spec Book I, p1-21.
  size_t strtab = 1;
  size_t dynstr = 1;
  size_t hash   = 0;

  // compute size of .symtab, .dynsym and .strtab
  SymbolCategory::const_iterator symbol;
  SymbolCategory::const_iterator symEnd = pSymbols.end();
  for (symbol = pSymbols.begin(); symbol != symEnd; ++symbol) {
    size_t str_size = (*symbol)->nameSize() + 1;
    if (isDynamicSymbol(**symbol, pOutput)) {
      ++dynsym;
      dynstr += str_size;
    }
    ++symtab;
    strtab += str_size;
  }

  ELFFileFormat* file_format = NULL;
  switch(pOutput.type()) {
    // compute size of .dynstr and .hash
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
    default:
      // TODO: not support yet
      return;
  }

  switch(pOutput.type()) {
    // compute size of .dynstr and .hash
    case Output::DynObj:
    case Output::Exec: {
      // add DT_NEED strings into .dynstr and .dynamic
      // Rules:
      //   1. ignore --no-add-needed
      //   2. force count in --no-as-needed
      //   3. judge --as-needed
      InputTree::const_bfs_iterator input, inputEnd = pLDInfo.inputs().bfs_end();
      for (input = pLDInfo.inputs().bfs_begin(); input != inputEnd; ++input) {
        if (Input::DynObj == (*input)->type()) {
          // --add-needed
          if ((*input)->attribute()->isAddNeeded()) {
            // --no-as-needed
            if (!(*input)->attribute()->isAsNeeded()) {
              dynstr += (*input)->name().size() + 1;
              dynamic().reserveNeedEntry();
            }
            // --as-needed
            else if ((*input)->isNeeded()) {
              dynstr += (*input)->name().size() + 1;
              dynamic().reserveNeedEntry();
            }
          }
        }
      } // for

      // compute .hash
      // Both Elf32_Word and Elf64_Word are 4 bytes
      hash = (2 + getHashBucketCount(dynsym, false) + dynsym) *
             sizeof(llvm::ELF::Elf32_Word);

      // set size
      dynstr += pOutput.name().size() + 1;
      if (32 == bitclass())
        file_format->getDynSymTab().setSize(dynsym*sizeof(llvm::ELF::Elf32_Sym));
      else
        file_format->getDynSymTab().setSize(dynsym*sizeof(llvm::ELF::Elf64_Sym));
      file_format->getDynStrTab().setSize(dynstr);
      file_format->getHashTab().setSize(hash);

    }
    /* fall through */
    case Output::Object: {
      if (32 == bitclass())
        file_format->getSymTab().setSize(symtab*sizeof(llvm::ELF::Elf32_Sym));
      else
        file_format->getSymTab().setSize(symtab*sizeof(llvm::ELF::Elf64_Sym));
      file_format->getStrTab().setSize(strtab);
      break;
    }
  } // end of switch

  // reserve fixed entries in the .dynamic section.
  if (Output::DynObj == pOutput.type() || Output::Exec == pOutput.type()) {
    // Because some entries in .dynamic section need information of .dynsym,
    // .dynstr, .symtab, .strtab and .hash, we can not reserve non-DT_NEEDED
    // entries until we get the size of the sections mentioned above
    dynamic().reserveEntries(pLDInfo, *file_format);
    file_format->getDynamic().setSize(dynamic().numOfBytes());
  }
}

/// emitRegNamePools - emit regular name pools - .symtab, .strtab
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
void GNULDBackend::emitRegNamePools(Output& pOutput,
                                    SymbolCategory& pSymbols,
                                    const Layout& pLayout,
                                    const MCLDInfo& pLDInfo)
{

  assert(pOutput.hasMemArea());

  bool sym_exist = false;
  HashTableType::entry_type* entry = 0;

  ELFFileFormat* file_format = NULL;
  switch(pOutput.type()) {
    // compute size of .dynstr and .hash
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
    default:
      // add first symbol into m_pSymIndexMap
      entry = m_pSymIndexMap->insert(NULL, sym_exist);
      entry->setValue(0);

      // TODO: not support yet
      return;
  }

  LDSection& symtab_sect = file_format->getSymTab();
  LDSection& strtab_sect = file_format->getStrTab();

  MemoryRegion* symtab_region = pOutput.memArea()->request(symtab_sect.offset(),
                                                           symtab_sect.size());
  MemoryRegion* strtab_region = pOutput.memArea()->request(strtab_sect.offset(),
                                                           strtab_sect.size());

  // set up symtab_region
  llvm::ELF::Elf32_Sym* symtab32 = NULL;
  llvm::ELF::Elf64_Sym* symtab64 = NULL;
  if (32 == bitclass())
    symtab32 = (llvm::ELF::Elf32_Sym*)symtab_region->start();
  else if (64 == bitclass())
    symtab64 = (llvm::ELF::Elf64_Sym*)symtab_region->start();
  else
    llvm::report_fatal_error(llvm::Twine("unsupported bitclass ") +
                             llvm::Twine(bitclass()) +
                             llvm::Twine(".\n"));
  // set up strtab_region
  char* strtab = (char*)strtab_region->start();
  strtab[0] = '\0';

  // initialize the first ELF symbol
  if (32 == bitclass()) {
    symtab32[0].st_name  = 0;
    symtab32[0].st_value = 0;
    symtab32[0].st_size  = 0;
    symtab32[0].st_info  = 0;
    symtab32[0].st_other = 0;
    symtab32[0].st_shndx = 0;
  }
  else { // must 64
    symtab64[0].st_name  = 0;
    symtab64[0].st_value = 0;
    symtab64[0].st_size  = 0;
    symtab64[0].st_info  = 0;
    symtab64[0].st_other = 0;
    symtab64[0].st_shndx = 0;
  }

  size_t symtabIdx = 1;
  size_t strtabsize = 1;
  // compute size of .symtab, .dynsym and .strtab
  SymbolCategory::iterator symbol;
  SymbolCategory::iterator symEnd = pSymbols.end();
  for (symbol = pSymbols.begin(); symbol != symEnd; ++symbol) {

     // maintain output's symbol and index map if building .o file
    if (Output::Object == pOutput.type()) {
      entry = m_pSymIndexMap->insert(NULL, sym_exist);
      entry->setValue(symtabIdx);
    }

    // FIXME: check the endian between host and target
    // write out symbol
    if (32 == bitclass()) {
      symtab32[symtabIdx].st_name  = strtabsize;
      symtab32[symtabIdx].st_value = getSymbolValue(**symbol);
      symtab32[symtabIdx].st_size  = getSymbolSize(**symbol);
      symtab32[symtabIdx].st_info  = getSymbolInfo(**symbol);
      symtab32[symtabIdx].st_other = (*symbol)->visibility();
      symtab32[symtabIdx].st_shndx = getSymbolShndx(**symbol, pLayout);
    }
    else { // must 64
      symtab64[symtabIdx].st_name  = strtabsize;
      symtab64[symtabIdx].st_value = getSymbolValue(**symbol);
      symtab64[symtabIdx].st_size  = getSymbolSize(**symbol);
      symtab64[symtabIdx].st_info  = getSymbolInfo(**symbol);
      symtab64[symtabIdx].st_other = (*symbol)->visibility();
      symtab64[symtabIdx].st_shndx = getSymbolShndx(**symbol, pLayout);
    }
    // write out string
    strcpy((strtab + strtabsize), (*symbol)->name());

    // write out
    // sum up counters
    ++symtabIdx;
    strtabsize += (*symbol)->nameSize() + 1;
  }
}

/// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
void GNULDBackend::emitDynNamePools(Output& pOutput,
                                    SymbolCategory& pSymbols,
                                    const Layout& pLayout,
                                    const MCLDInfo& pLDInfo)
{
  assert(pOutput.hasMemArea());
  ELFFileFormat* file_format = NULL;

  bool sym_exist = false;
  HashTableType::entry_type* entry = 0;

  switch(pOutput.type()) {
    // compute size of .dynstr and .hash
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
    default:
      // TODO: not support yet
      return;
  }

  LDSection& symtab_sect = file_format->getDynSymTab();
  LDSection& strtab_sect = file_format->getDynStrTab();
  LDSection& hash_sect   = file_format->getHashTab();
  LDSection& dyn_sect    = file_format->getDynamic();

  MemoryRegion* symtab_region = pOutput.memArea()->request(symtab_sect.offset(),
                                                           symtab_sect.size());
  MemoryRegion* strtab_region = pOutput.memArea()->request(strtab_sect.offset(),
                                                           strtab_sect.size());
  MemoryRegion* hash_region = pOutput.memArea()->request(hash_sect.offset(),
                                                         hash_sect.size());
  MemoryRegion* dyn_region = pOutput.memArea()->request(dyn_sect.offset(),
                                                        dyn_sect.size());
  // set up symtab_region
  llvm::ELF::Elf32_Sym* symtab32 = NULL;
  llvm::ELF::Elf64_Sym* symtab64 = NULL;
  if (32 == bitclass())
    symtab32 = (llvm::ELF::Elf32_Sym*)symtab_region->start();
  else if (64 == bitclass())
    symtab64 = (llvm::ELF::Elf64_Sym*)symtab_region->start();
  else
    llvm::report_fatal_error(llvm::Twine("unsupported bitclass ") +
                             llvm::Twine(bitclass()) +
                             llvm::Twine(".\n"));

  // initialize the first ELF symbol
  if (32 == bitclass()) {
    symtab32[0].st_name  = 0;
    symtab32[0].st_value = 0;
    symtab32[0].st_size  = 0;
    symtab32[0].st_info  = 0;
    symtab32[0].st_other = 0;
    symtab32[0].st_shndx = 0;
  }
  else { // must 64
    symtab64[0].st_name  = 0;
    symtab64[0].st_value = 0;
    symtab64[0].st_size  = 0;
    symtab64[0].st_info  = 0;
    symtab64[0].st_other = 0;
    symtab64[0].st_shndx = 0;
  }
  // set up strtab_region
  char* strtab = (char*)strtab_region->start();
  strtab[0] = '\0';

  // add the first symbol into m_pSymIndexMap
  entry = m_pSymIndexMap->insert(NULL, sym_exist);
  entry->setValue(0);

  size_t symtabIdx = 1;
  size_t strtabsize = 1;

  // emit of .dynsym, and .dynstr
  SymbolCategory::iterator symbol;
  SymbolCategory::iterator symEnd = pSymbols.end();
  for (symbol = pSymbols.begin(); symbol != symEnd; ++symbol) {
    if (!isDynamicSymbol(**symbol, pOutput))
      continue;

    // maintain output's symbol and index map
    entry = m_pSymIndexMap->insert(*symbol, sym_exist);
    entry->setValue(symtabIdx);

    // FIXME: check the endian between host and target
    // write out symbol
    if (32 == bitclass()) {
      symtab32[symtabIdx].st_name  = strtabsize;
      symtab32[symtabIdx].st_value = (*symbol)->value();
      symtab32[symtabIdx].st_size  = getSymbolSize(**symbol);
      symtab32[symtabIdx].st_info  = getSymbolInfo(**symbol);
      symtab32[symtabIdx].st_other = (*symbol)->visibility();
      symtab32[symtabIdx].st_shndx = getSymbolShndx(**symbol, pLayout);
    }
    else { // must 64
      symtab64[symtabIdx].st_name  = strtabsize;
      symtab64[symtabIdx].st_value = (*symbol)->value();
      symtab64[symtabIdx].st_size  = getSymbolSize(**symbol);
      symtab64[symtabIdx].st_info  = getSymbolInfo(**symbol);
      symtab64[symtabIdx].st_other = (*symbol)->visibility();
      symtab64[symtabIdx].st_shndx = getSymbolShndx(**symbol, pLayout);
    }
    // write out string
    strcpy((strtab + strtabsize), (*symbol)->name());

    // sum up counters
    ++symtabIdx;
    strtabsize += (*symbol)->nameSize() + 1;
  }

  // emit DT_NEED
  // add DT_NEED strings into .dynstr
  // Rules:
  //   1. ignore --no-add-needed
  //   2. force count in --no-as-needed
  //   3. judge --as-needed
  ELFDynamic::iterator dt_need = dynamic().needBegin();
  InputTree::const_bfs_iterator input, inputEnd = pLDInfo.inputs().bfs_end();
  for (input = pLDInfo.inputs().bfs_begin(); input != inputEnd; ++input) {
    if (Input::DynObj == (*input)->type()) {
      // --add-needed
      if ((*input)->attribute()->isAddNeeded()) {
        // --no-as-needed
        if (!(*input)->attribute()->isAsNeeded()) {
          strcpy((strtab + strtabsize), (*input)->name().c_str());
          (*dt_need)->setValue(llvm::ELF::DT_NEEDED, strtabsize);
          strtabsize += (*input)->name().size() + 1;
          ++dt_need;
        }
        // --as-needed
        else if ((*input)->isNeeded()) {
          strcpy((strtab + strtabsize), (*input)->name().c_str());
          (*dt_need)->setValue(llvm::ELF::DT_NEEDED, strtabsize);
          strtabsize += (*input)->name().size() + 1;
          ++dt_need;
        }
      }
    }
  } // for

  // emit soname
  // initialize value of ELF .dynamic section
  dynamic().applySoname(strtabsize);
  dynamic().applyEntries(pLDInfo, *file_format);
  dynamic().emit(dyn_sect, *dyn_region);

  strcpy((strtab + strtabsize), pOutput.name().c_str());
  strtabsize += pOutput.name().size() + 1;

  // emit hash table
  // FIXME: this verion only emit SVR4 hash section.
  //        Please add GNU new hash section

  // both 32 and 64 bits hash table use 32-bit entry
  // set up hash_region
  uint32_t* word_array = (uint32_t*)hash_region->start();
  uint32_t& nbucket = word_array[0];
  uint32_t& nchain  = word_array[1];

  nbucket = getHashBucketCount(symtabIdx, false);
  nchain  = symtabIdx;

  uint32_t* bucket = (word_array + 2);
  uint32_t* chain  = (bucket + nbucket);

  // initialize bucket
  bzero((void*)bucket, nbucket);

  StringHash<ELF> hash_func;

  if (32 == bitclass()) {
    for (size_t sym_idx=0; sym_idx < symtabIdx; ++sym_idx) {
      llvm::StringRef name(strtab + symtab32[sym_idx].st_name);
      size_t bucket_pos = hash_func(name) % nbucket;
      chain[sym_idx] = bucket[bucket_pos];
      bucket[bucket_pos] = sym_idx;
    }
  }
  else if (64 == bitclass()) {
    for (size_t sym_idx=0; sym_idx < symtabIdx; ++sym_idx) {
      llvm::StringRef name(strtab + symtab64[sym_idx].st_name);
      size_t bucket_pos = hash_func(name) % nbucket;
      chain[sym_idx] = bucket[bucket_pos];
      bucket[bucket_pos] = sym_idx;
    }
  }
}

/// getSectionOrder
unsigned int GNULDBackend::getSectionOrder(const Output& pOutput,
                                           const LDSection& pSectHdr) const
{
  // NULL section should be the "1st" section
  if (LDFileFormat::Null == pSectHdr.kind())
    return 0;

  // if the section is not ALLOC, lay it out until the last possible moment
  if (0 == (pSectHdr.flag() & llvm::ELF::SHF_ALLOC))
    return SHO_UNDEFINED;

  bool is_write = (pSectHdr.flag() & llvm::ELF::SHF_WRITE) != 0;
  bool is_exec = (pSectHdr.flag() & llvm::ELF::SHF_EXECINSTR) != 0;
  ELFFileFormat* file_format = NULL;
  switch (pOutput.type()) {
    case Output::DynObj:
      file_format = getDynObjFileFormat();
      break;
    case Output::Exec:
      file_format = getExecFileFormat();
      break;
    case Output::Object:
    default:
      assert(0 && "Not support yet.\n");
      break;
  }

  // TODO: need to take care other possible output sections
  switch (pSectHdr.kind()) {
    case LDFileFormat::Regular:
      if (is_exec) {
        if (&pSectHdr == &file_format->getInit())
          return SHO_INIT;
        if (&pSectHdr == &file_format->getFini())
          return SHO_FINI;
        return SHO_TEXT;
      } else if (!is_write) {
        return SHO_RO;
      } else {
        if (pSectHdr.type() == llvm::ELF::SHT_PREINIT_ARRAY ||
            pSectHdr.type() == llvm::ELF::SHT_INIT_ARRAY ||
            pSectHdr.type() == llvm::ELF::SHT_FINI_ARRAY ||
            &pSectHdr == &file_format->getCtors() ||
            &pSectHdr == &file_format->getDtors())
          return SHO_RELRO;

        return SHO_DATA;
      }

    case LDFileFormat::BSS:
      return SHO_BSS;

    case LDFileFormat::NamePool:
      if (&pSectHdr == &file_format->getDynamic())
        return SHO_RELRO;
      return SHO_NAMEPOOL;

    case LDFileFormat::Relocation:
      if (&pSectHdr == &file_format->getRelPlt() ||
          &pSectHdr == &file_format->getRelaPlt())
        return SHO_REL_PLT;
      return SHO_RELOCATION;

    // get the order from target for target specific sections
    case LDFileFormat::Target:
      return getTargetSectionOrder(pOutput, pSectHdr);

    // handle .interp
    case LDFileFormat::Note:
      return SHO_INTERP;

    case LDFileFormat::Exception:
      return SHO_EHFRAME;

    case LDFileFormat::MetaData:
    case LDFileFormat::Debug:
    default:
      return SHO_UNDEFINED;
  }
}

/// getSymbolSize
uint64_t GNULDBackend::getSymbolSize(const LDSymbol& pSymbol) const
{
  // @ref Google gold linker: symtab.cc: 2780
  // undefined and dynamic symbols should have zero size.
  if (pSymbol.isDyn() || pSymbol.desc() == ResolveInfo::Undefined)
    return 0x0;
  return pSymbol.resolveInfo()->size();
}

/// getSymbolInfo
uint64_t GNULDBackend::getSymbolInfo(const LDSymbol& pSymbol) const
{
  // set binding
  uint8_t bind = 0x0;
  if (pSymbol.resolveInfo()->isLocal())
    bind = llvm::ELF::STB_LOCAL;
  else if (pSymbol.resolveInfo()->isGlobal())
    bind = llvm::ELF::STB_GLOBAL;
  else if (pSymbol.resolveInfo()->isWeak())
    bind = llvm::ELF::STB_WEAK;
  else if (pSymbol.resolveInfo()->isAbsolute()) {
    // (Luba) Is a absolute but not global (weak or local) symbol meaningful?
    bind = llvm::ELF::STB_GLOBAL;
  }

  if (pSymbol.visibility() == llvm::ELF::STV_INTERNAL ||
      pSymbol.visibility() == llvm::ELF::STV_HIDDEN)
    bind = llvm::ELF::STB_LOCAL;

  return (pSymbol.resolveInfo()->type() | (bind << 4));
}

/// getSymbolValue - this function is called after layout()
uint64_t GNULDBackend::getSymbolValue(const LDSymbol& pSymbol) const
{
  if (pSymbol.isDyn())
    return 0x0;

  return pSymbol.value();
}

/// getSymbolShndx - this function is called after layout()
uint64_t
GNULDBackend::getSymbolShndx(const LDSymbol& pSymbol, const Layout& pLayout) const
{
  if (pSymbol.resolveInfo()->isAbsolute())
    return llvm::ELF::SHN_ABS;
  if (pSymbol.resolveInfo()->isCommon())
    return llvm::ELF::SHN_COMMON;
  if (pSymbol.resolveInfo()->isUndef() || pSymbol.isDyn())
    return llvm::ELF::SHN_UNDEF;

  if (pSymbol.resolveInfo()->isLocal()) {
    switch (pSymbol.type()) {
      case ResolveInfo::NoType:
      case ResolveInfo::File:
        return llvm::ELF::SHN_ABS;
    }
  }

  assert(pSymbol.hasFragRef());
  return pLayout.getOutputLDSection(*pSymbol.fragRef()->frag())->index();
}

/// getSymbolIdx - called by emitRelocation to get the ouput symbol table index
size_t GNULDBackend::getSymbolIdx(LDSymbol* pSymbol) const
{
   HashTableType::iterator entry = m_pSymIndexMap->find(pSymbol);
   return entry.getEntry()->value();
}

/// emitProgramHdrs - emit ELF program headers
void GNULDBackend::emitProgramHdrs(Output& pOutput)
{
  assert(NULL != pOutput.context());
  createProgramHdrs(*pOutput.context());

  if (32 == bitclass())
    writeELF32ProgramHdrs(pOutput);
  else
    writeELF64ProgramHdrs(pOutput);
}

/// createProgramHdrs - base on output sections to create the program headers
void GNULDBackend::createProgramHdrs(LDContext& pContext)
{
  // make PT_PHDR
  m_ELFSegmentTable.produce(llvm::ELF::PT_PHDR);

  // make PT_INTERP
  LDSection* interp = pContext.getSection(".interp");
  if (NULL != interp) {
    ELFSegment* interp_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_INTERP);
    interp_seg->addSection(interp);
    interp_seg->setAlign(bitclass() / 8);
  }

  uint32_t cur_seg_flag, prev_seg_flag = getSegmentFlag(0);
  uint64_t padding = 0;
  ELFSegment* load_seg = NULL;
  // make possible PT_LOAD segments
  LDContext::sect_iterator sect, sect_end = pContext.sectEnd();
  for (sect = pContext.sectBegin(); sect != sect_end; ++sect) {
    if (0 == ((*sect)->flag() & llvm::ELF::SHF_ALLOC) &&
        LDFileFormat::Null != (*sect)->kind())
      continue;

    // FIXME: Now only separate writable and non-writable PT_LOAD
    cur_seg_flag = getSegmentFlag((*sect)->flag());
    if ((prev_seg_flag & llvm::ELF::PF_W) ^ (cur_seg_flag & llvm::ELF::PF_W) ||
         LDFileFormat::Null == (*sect)->kind()) {
      // create new PT_LOAD segment
      load_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_LOAD);
      load_seg->setAlign(pagesize());

      // check if this segment needs padding
      padding = 0;
      if (((*sect)->offset() & (load_seg->align() - 1)) != 0)
        padding = load_seg->align();
    }

    assert(NULL != load_seg);
    load_seg->addSection(*sect);
    load_seg->updateFlag(cur_seg_flag);

    // FIXME: set section's vma
    // need to handle start vma for user-defined one or for executable.
    (*sect)->setAddr((*sect)->offset() + padding);

    prev_seg_flag = cur_seg_flag;
  }

  // make PT_DYNAMIC
  LDSection* dynamic = pContext.getSection(".dynamic");
  if (NULL != dynamic) {
    ELFSegment* dyn_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_DYNAMIC);
    dyn_seg->setFlag(llvm::ELF::PF_R | llvm::ELF::PF_W);
    dyn_seg->addSection(dynamic);
    dyn_seg->setAlign(bitclass() / 8);
  }

  // update segment info
  uint64_t file_size = 0;
  ELFSegmentFactory::iterator seg, seg_end = m_ELFSegmentTable.end();
  for (seg = m_ELFSegmentTable.begin(); seg != seg_end; ++seg) {
    ELFSegment& segment = *seg;

    // update PT_PHDR
    if (llvm::ELF::PT_PHDR == segment.type()) {
      uint64_t offset, phdr_size;
      if (32 == bitclass()) {
        offset = sizeof(llvm::ELF::Elf32_Ehdr);
        phdr_size = sizeof(llvm::ELF::Elf32_Phdr);
      }
      else {
        offset = sizeof(llvm::ELF::Elf64_Ehdr);
        phdr_size = sizeof(llvm::ELF::Elf64_Phdr);
      }
      segment.setOffset(offset);
      segment.setVaddr(offset);
      segment.setPaddr(segment.vaddr());
      segment.setFilesz(numOfSegments() * phdr_size);
      segment.setMemsz(numOfSegments() * phdr_size);
      segment.setAlign(bitclass() / 8);
      continue;
    }

    assert(NULL != segment.getFirstSection());
    segment.setOffset(segment.getFirstSection()->offset());
    segment.setVaddr(segment.getFirstSection()->addr());
    segment.setPaddr(segment.vaddr());

    const LDSection* last_sect = segment.getLastSection();
    assert(NULL != last_sect);
    file_size = last_sect->offset() - segment.offset();
    if (LDFileFormat::BSS != last_sect->kind())
      file_size += last_sect->size();
    segment.setFilesz(file_size);

    segment.setMemsz(last_sect->addr() - segment.vaddr() + last_sect->size());
  }
}

/// writeELF32ProgramHdrs - write out the ELF32 program headers
void GNULDBackend::writeELF32ProgramHdrs(Output& pOutput)
{
  assert(pOutput.hasMemArea());

  uint64_t start_offset, phdr_size;

  start_offset = sizeof(llvm::ELF::Elf32_Ehdr);
  phdr_size = sizeof(llvm::ELF::Elf32_Phdr);
  // Program header must start directly after ELF header
  MemoryRegion *region = pOutput.memArea()->request(start_offset,
                                                    numOfSegments()*phdr_size);

  llvm::ELF::Elf32_Phdr* phdr = (llvm::ELF::Elf32_Phdr*)region->start();

  size_t index = 0;
  ELFSegmentFactory::iterator seg, segEnd = m_ELFSegmentTable.end();
  for (seg = m_ELFSegmentTable.begin(); seg != segEnd; ++seg, ++index) {
    phdr[index].p_type   = (*seg).type();
    phdr[index].p_flags  = (*seg).flag();
    phdr[index].p_offset = (*seg).offset();
    phdr[index].p_vaddr  = (*seg).vaddr();
    phdr[index].p_paddr  = (*seg).paddr();
    phdr[index].p_filesz = (*seg).filesz();
    phdr[index].p_memsz  = (*seg).memsz();
    phdr[index].p_align  = (*seg).align();
  }
}

/// writeELF64ProgramHdrs - write out the ELF64 program headers
void GNULDBackend::writeELF64ProgramHdrs(Output& pOutput)
{
  assert(pOutput.hasMemArea());

  uint64_t start_offset, phdr_size;

  start_offset = sizeof(llvm::ELF::Elf64_Ehdr);
  phdr_size = sizeof(llvm::ELF::Elf64_Phdr);
  // Program header must start directly after ELF header
  MemoryRegion *region = pOutput.memArea()->request(start_offset,
                                                    numOfSegments() *phdr_size);
  llvm::ELF::Elf64_Phdr* phdr = (llvm::ELF::Elf64_Phdr*)region->start();

  size_t index = 0;
  ELFSegmentFactory::iterator seg, segEnd = m_ELFSegmentTable.end();
  for (seg = m_ELFSegmentTable.begin(); seg != segEnd; ++seg, ++index) {
    phdr[index].p_type   = (*seg).type();
    phdr[index].p_flags  = (*seg).flag();
    phdr[index].p_offset = (*seg).offset();
    phdr[index].p_vaddr  = (*seg).vaddr();
    phdr[index].p_paddr  = (*seg).paddr();
    phdr[index].p_filesz = (*seg).filesz();
    phdr[index].p_memsz  = (*seg).memsz();
    phdr[index].p_align  = (*seg).align();
  }
}

/// preLayout - Backend can do any needed modification before layout
void GNULDBackend::preLayout(const Output& pOutput,
                             const MCLDInfo& pLDInfo,
                             MCLinker& pLinker)
{
  // prelayout target first
  doPreLayout(pOutput, pLDInfo, pLinker);
}

/// postLayout -Backend can do any needed modification after layout
void GNULDBackend::postLayout(const Output& pOutput,
                              const MCLDInfo& pInfo,
                              MCLinker& pLinker)
{
  // post layout target first
  doPostLayout(pOutput, pInfo, pLinker);
}

/// getHashBucketCount - calculate hash bucket count.
/// @ref Google gold linker, dynobj.cc:791
unsigned GNULDBackend::getHashBucketCount(unsigned pNumOfSymbols,
                                          bool pIsGNUStyle)
{
  // @ref Google gold, dynobj.cc:loc 791
  static const unsigned int buckets[] =
  {
    1, 3, 17, 37, 67, 97, 131, 197, 263, 521, 1031, 2053, 4099, 8209,
    16411, 32771, 65537, 131101, 262147
  };
  const unsigned buckets_count = sizeof buckets / sizeof buckets[0];

  unsigned int result = 1;
  for (unsigned i = 0; i < buckets_count; ++i) {
    if (pNumOfSymbols < buckets[i])
      break;
    result = buckets[i];
  }

  if (pIsGNUStyle && result < 2)
    result = 2;

  return result;
}

/// isDynamicSymbol
/// @ref Google gold linker: symtab.cc:311
bool GNULDBackend::isDynamicSymbol(const LDSymbol& pSymbol,
                                   const Output& pOutput)
{
  // If a local symbol is in the LDContext's symbol table, it's a real local
  // symbol. We should not add it
  if (pSymbol.binding() == ResolveInfo::Local)
    return false;

  // If we are building shared object, and the visibility is external, we
  // need to add it.
  if (Output::DynObj == pOutput.type())
    if (pSymbol.resolveInfo()->visibility() == ResolveInfo::Default ||
        pSymbol.resolveInfo()->visibility() == ResolveInfo::Protected)
      return true;

  return false;
}
