//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ELF.h>
#include <mcld/Target/GNULDBackend.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/MCLDInputTree.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/Layout.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// non-member functions
static unsigned int
hash_bucket_count(unsigned int pNumOfSymbols, bool pIsGNUStyle)
{
  // @ref Google gold, dynobj.cc:loc 791
  static const unsigned int buckets[] =
  {
    1, 3, 17, 37, 67, 97, 131, 197, 263, 521, 1031, 2053, 4099, 8209,
    16411, 32771, 65537, 131101, 262147
  };
  const int buckets_count = sizeof buckets / sizeof buckets[0];

  unsigned int result = 1;
  for (unsigned int i = 0; i < buckets_count; ++i) {
    if (pNumOfSymbols < buckets[i])
      break;
    result = buckets[i];
  }

  if (pIsGNUStyle && result < 2)
    result = 2;

  return result;
}

//===----------------------------------------------------------------------===//
// GNULDBackend
GNULDBackend::GNULDBackend()
  : m_pArchiveReader(0),
    m_pObjectReader(0),
    m_pDynObjReader(0),
    m_pObjectWriter(0),
    m_pDynObjWriter(0),
    m_pDynObjFileFormat(0),
    m_pExecFileFormat(0) {
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
}

size_t GNULDBackend::sectionStartOffset() const
{
  // FIXME: use fixed offset, we need 10 segments by default
  return sizeof(llvm::ELF::Elf64_Ehdr)+10*sizeof(llvm::ELF::Elf64_Phdr);
}

bool GNULDBackend::initArchiveReader(MCLinker&)
{
  if (0 == m_pArchiveReader)
    // FIXME: What is the correct ctor for GNUArchiveReader?
    m_pArchiveReader = new GNUArchiveReader();
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
    m_pExecFileFormat = new ELFExecFileFormat();

  // initialize standard sections
  m_pExecFileFormat->initStdSections(pMCLinker);
  return true;
}

bool GNULDBackend::initDynObjSections(MCLinker& pMCLinker)
{
  if (0 == m_pDynObjFileFormat)
    m_pDynObjFileFormat = new ELFDynObjFileFormat();

  // initialize standard sections
  m_pDynObjFileFormat->initStdSections(pMCLinker);
  return true;
}

GNUArchiveReader *GNULDBackend::getArchiveReader()
{
  assert(0 != m_pArchiveReader);
  return m_pArchiveReader;
}

ELFObjectReader *GNULDBackend::getObjectReader()
{
  assert(0 != m_pObjectReader);
  return m_pObjectReader;
}

ELFDynObjReader *GNULDBackend::getDynObjReader()
{
  assert(0 != m_pDynObjReader);
  return m_pDynObjReader;
}

ELFObjectWriter *GNULDBackend::getObjectWriter()
{
  // TODO
  return NULL;
}

ELFDynObjWriter *GNULDBackend::getDynObjWriter()
{
  assert(0 != m_pDynObjWriter);
  return m_pDynObjWriter;
}


ELFDynObjFileFormat* GNULDBackend::getDynObjFileFormat()
{
  assert(0 != m_pDynObjFileFormat);
  return m_pDynObjFileFormat;
}

ELFExecFileFormat* GNULDBackend::getExecFileFormat()
{
  assert(0 != m_pExecFileFormat);
  return m_pExecFileFormat;
}

/// sizeNamePools - compute the size of regular name pools
/// In ELF executable files, regular name pools are .symtab, .strtab.,
/// .dynsym, .dynstr, and .hash
void
GNULDBackend::sizeNamePools(const Output& pOutput,
                            const MCLDInfo& pLDInfo)
{
  size_t symtab = 0;
  size_t dynsym = 0;
  size_t strtab = 0;
  size_t dynstr = 0;
  size_t hash   = 0;

  // compute size of .symtab, .dynsym and .strtab
  LDContext::const_sym_iterator symbol;
  LDContext::const_sym_iterator symEnd = pOutput.context()->symEnd();
  for (symbol = pOutput.context()->symBegin(); symbol != symEnd; ++symbol) {
    size_t str_size = (*symbol)->nameSize() + 1;
    if ((*symbol)->binding() != ResolveInfo::Local) {
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
      // add DT_NEED strings into .dynstr
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
            if (!(*input)->attribute()->isAsNeeded())
              dynstr += (*input)->name().size() + 1;
            // --as-needed
            else if ((*input)->isNeeded())
              dynstr += (*input)->name().size() + 1;
          }
        }
      } // for

      // compute .hash
      // Both Elf32_Word and Elf64_Word are 4 bytes
      hash = (2 + hash_bucket_count(dynsym, false) + dynsym) * sizeof(llvm::ELF::Elf32_Word);

      // set size
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
  }
}

/// emitRegNamePools - emit regular name pools - .symtab, .strtab
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
void GNULDBackend::emitRegNamePools(Output& pOutput,
                                    const Layout& pLayout,
                                    const MCLDInfo& pLDInfo)
{

  assert(pOutput.hasMemArea());
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

  LDSection& symtab_sect = file_format->getSymTab();
  LDSection& strtab_sect = file_format->getStrTab();

  MemoryRegion* symtab_region = pOutput.memArea()->request(symtab_sect.offset(),
                                                           symtab_sect.size(),
                                                           true);
  MemoryRegion* strtab_region = pOutput.memArea()->request(strtab_sect.offset(),
                                                           strtab_sect.size(),
                                                           true);

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

  size_t symtabIdx = 0;
  size_t strtabsize = 0;

  // compute size of .symtab, .dynsym and .strtab
  LDContext::const_sym_iterator symbol;
  LDContext::const_sym_iterator symEnd = pOutput.context()->symEnd();
  for (symbol = pOutput.context()->symBegin(); symbol != symEnd; ++symbol) {
    // write out symbol
    if (32 == bitclass()) {
      symtab32[symtabIdx].st_name  = strtabsize;
      symtab32[symtabIdx].st_value = (*symbol)->value();
      symtab32[symtabIdx].st_size  = (*symbol)->size();
      symtab32[symtabIdx].st_info  = getSymbolInfo(**symbol);
      symtab32[symtabIdx].st_other = (*symbol)->visibility();
      symtab32[symtabIdx].st_shndx = getSymbolShndx(**symbol, pLayout);
    }
    else { // must 64
      symtab64[symtabIdx].st_name  = strtabsize;
      symtab64[symtabIdx].st_value = (*symbol)->value();
      symtab64[symtabIdx].st_size  = (*symbol)->size();
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

  symtab_region->sync();
  strtab_region->sync();
}

/// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
void GNULDBackend::emitDynNamePools(Output& pOutput,
                                    const Layout& pLayout,
                                    const MCLDInfo& pLDInfo)
{
  // FIXME
}

/// getSectionOrder
unsigned int GNULDBackend::getSectionOrder(const LDSection& pSectHdr) const
{
  if (LDFileFormat::Target == pSectHdr.kind())
    return getTargetSectionOrder(pSectHdr);
  // TODO
  return ~(0U);
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

  return (pSymbol.resolveInfo()->type() | (bind << 4));
}

uint64_t
GNULDBackend::getSymbolShndx(const LDSymbol& pSymbol, const Layout& pLayout) const
{
  if (pSymbol.resolveInfo()->isAbsolute())
    return llvm::ELF::SHN_ABS;
  if (pSymbol.resolveInfo()->isCommon())
    return llvm::ELF::SHN_COMMON;
  if (pSymbol.resolveInfo()->isUndef())
    return llvm::ELF::SHN_UNDEF;
  return 0x0;
  // FIXME: when Layout is ready, open the comment.
  // return pLayout.getOutputLDSection(*pSymbol.fragRef()->frag())->index();
}

