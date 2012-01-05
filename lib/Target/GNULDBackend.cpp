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
#include <string>
#include <cassert>

using namespace mcld;

//===----------------------------------------------------------------------===//
// non-member functions
namespace {

unsigned int
hash_bucket_count(unsigned int pNumOfSymbols, bool pIsGNUStyle)
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

void createDynamicEntry(llvm::ELF::Elf32_Sword pTag,
                        llvm::MCSectionData* SectionData)
{
  llvm::ELF::Elf32_Dyn Entry_ptr;
  Entry_ptr.d_tag = pTag;

  llvm::MCDataFragment* Fragment = new llvm::MCDataFragment(SectionData);
  Fragment->getContents() +=
   llvm::StringRef(reinterpret_cast<char*>(&Entry_ptr),
                   sizeof(llvm::ELF::Elf32_Dyn));
}

void
emitDTNULL(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(0);
}

void
emitDTPLTRelSZ(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getRelPlt().size());
}

void
emitDTPLTGOT(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getGOT().addr());
}

void emitDTHash(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getHashTab().addr());
}

void
emitDTStrTab(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getStrTab().addr());
}

void
emitDTSymTab(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getSymTab().addr());
}

void
emitDTRela(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat) {
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getRelaDyn().addr());
}

void
emitDTRelaSZ(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getRelaDyn().size());
}

void
emitDTStrSZ(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getStrTab().size());
}

void
emitDTSymEnt(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val =
    static_cast<llvm::ELF::Elf32_Word>(sizeof(llvm::ELF::Elf32_Sym));
}

void
emitDTInit(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getInit().addr());
}

void
emitDTFini(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getFini().addr());
}

void
emitDTSymbolic(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(0);
}

void
emitDTRel(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getRelDyn().addr());
}

void
emitDTRelSZ(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getRelDyn().size());
}

void
emitDTRelEnt(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val =
    static_cast<llvm::ELF::Elf32_Word>(sizeof(llvm::ELF::Elf32_Rel));
}

void
emitDTPLTRel(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
   buffer->d_un.d_ptr =
     static_cast<llvm::ELF::Elf32_Addr>(llvm::ELF::DT_REL);
}

void
emitDTJMPRel(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getRelPlt().addr());
}

void
emitDTInitArray(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getInitArray().addr());
}

void
emitDTFiniArray(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_ptr = static_cast<llvm::ELF::Elf32_Addr>(
                       FileFormat->getFiniArray().addr());
}

void
emitDTInitArraySZ(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getInitArray().size());
}

void
emitDTFiniArraySZ(llvm::ELF::Elf32_Dyn* buffer, ELFDynObjFileFormat* FileFormat)
{
  buffer->d_un.d_val = static_cast<llvm::ELF::Elf32_Word>(
                       FileFormat->getFiniArray().size());
}

const unsigned int dynamic_type_num = 33;
typedef void (*DynamicEntryFn)(llvm::ELF::Elf32_Dyn*, ELFDynObjFileFormat*);
DynamicEntryFn emitDynamicEntry[] = {
  emitDTNULL,
  0, //DT_NEEDED
  emitDTPLTRelSZ,
  emitDTPLTGOT,
  emitDTHash,
  emitDTStrTab,
  emitDTSymTab,
  emitDTRela,
  emitDTRelaSZ,
  0, // DT_RELAENT
  emitDTStrSZ,
  emitDTSymEnt,
  emitDTInit,
  emitDTFini,
  0, // DT_SONAME
  0, //DT_RPATH
  emitDTSymbolic,
  emitDTRel,
  emitDTRelSZ,
  emitDTRelEnt,
  emitDTPLTRel,
  0, // DT_DEBUG
  0, // DT_TEXTREL
  emitDTJMPRel,
  0, // DT_BIND_NOW
  emitDTInitArray,
  emitDTFiniArray,
  emitDTInitArraySZ,
  emitDTFiniArraySZ,
  0, // DT_RUNPATH
  0, // DT_FLAGS
  0, // DT_ENCODING
  0, // DT_PREINIT_ARRAY
  0 // DT_PREINIT_ARRAYSZ
};

} // end namespace

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

// sizeDynamic - compute the size of .dynamic section
void GNULDBackend::sizeDynamic(Output& pOutput,
                               const MCLDInfo& pLDInfo)
{
  if (Output::Object == pOutput.type()) {
    llvm::report_fatal_error(
    "Relocatable object file should not have .dynamic section!");
  }

  unsigned int type = pOutput.type();
  size_t size = 0;

  InputTree::const_bfs_iterator input = pLDInfo.inputs().bfs_begin();
  InputTree::const_bfs_iterator inputEnd = pLDInfo.inputs().bfs_end();

  ELFDynObjFileFormat* FileFormat = getDynObjFileFormat();
  assert(FileFormat && "DynObjFileFormat is NULL!");

  LDSection* dynamic = &(FileFormat->getDynamic());
  assert(dynamic && ".dynamic section donesn't exist!");

  llvm::MCSectionData* SectionData = dynamic->getSectionData();
  assert(SectionData && ".dynamic has no MCSectionData");

  llvm::MCDataFragment* Fragment = 0;

  llvm::ELF::Elf32_Dyn Entry;
  Entry.d_tag = 0;
  Entry.d_un.d_val = 0;

  while (input != inputEnd) {
    // --add-needed
    if ((*input)->attribute()->isAddNeeded()) {
      // --no-as-need
      if (!(*input)->attribute()->isAsNeeded()) {
        Fragment = new llvm::MCDataFragment(SectionData);

        Entry.d_tag = llvm::ELF::DT_NEEDED;
        //FIXME: d_un.d_val needs an index into string table.
        Entry.d_un.d_val = 0;

        Fragment->getContents() += llvm::StringRef(
          reinterpret_cast<char*>(&Entry), sizeof(llvm::ELF::Elf32_Dyn));
      }

      // --as-needed
      else if ((*input)->isNeeded()) {
        Fragment = new llvm::MCDataFragment(SectionData);

        Entry.d_tag = llvm::ELF::DT_NEEDED;
        //FIXME: d_un.d_val needs an index into string table.
        Entry.d_un.d_val = 0;

        Fragment->getContents() += llvm::StringRef(
          reinterpret_cast<char*>(&Entry), sizeof(llvm::ELF::Elf32_Dyn));
      }
    }

    ++input;
  }

  if (type == Output::DynObj) {
    // DT_SONAME
    Fragment = new llvm::MCDataFragment(SectionData);

    Entry.d_tag = llvm::ELF::DT_SONAME;
    //FIXME: d_un.d_val needs an index into string table.
    Entry.d_un.d_val = 0;

    Fragment->getContents() += llvm::StringRef(
              reinterpret_cast<char*>(&Entry),sizeof(llvm::ELF::Elf32_Dyn));

    if (pLDInfo.options().Bsymbolic())
      createDynamicEntry(llvm::ELF::DT_SYMBOLIC, SectionData);
  }

  createDynamicEntry(llvm::ELF::DT_INIT, SectionData);
  createDynamicEntry(llvm::ELF::DT_FINI, SectionData);
  createDynamicEntry(llvm::ELF::DT_INIT_ARRAY, SectionData);
  createDynamicEntry(llvm::ELF::DT_FINI_ARRAY, SectionData);
  createDynamicEntry(llvm::ELF::DT_INIT_ARRAYSZ, SectionData);
  createDynamicEntry(llvm::ELF::DT_FINI_ARRAYSZ, SectionData);
  createDynamicEntry(llvm::ELF::DT_HASH, SectionData);
  createDynamicEntry(llvm::ELF::DT_STRTAB, SectionData);
  createDynamicEntry(llvm::ELF::DT_SYMTAB, SectionData);
  createDynamicEntry(llvm::ELF::DT_STRSZ, SectionData);
  createDynamicEntry(llvm::ELF::DT_SYMENT, SectionData);
  createDynamicEntry(llvm::ELF::DT_PLTGOT, SectionData);
  createDynamicEntry(llvm::ELF::DT_PLTRELSZ, SectionData);
  createDynamicEntry(llvm::ELF::DT_PLTREL, SectionData);
  createDynamicEntry(llvm::ELF::DT_JMPREL, SectionData);
  createDynamicEntry(llvm::ELF::DT_REL, SectionData);
  createDynamicEntry(llvm::ELF::DT_RELSZ, SectionData);
  createDynamicEntry(llvm::ELF::DT_RELENT, SectionData);
  createDynamicEntry(llvm::ELF::DT_NULL, SectionData);
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

// emitDynamic - emit .dynamic section
void GNULDBackend::emitDynamic(Output& pOutput)
{
  assert(m_pDynObjFileFormat && "m_pDynObjFileFormat is NULL!");

  LDSection* dynamic = &(m_pDynObjFileFormat->getDynamic());
  assert(dynamic && ".dynamic section donesn't exist!");

  llvm::MCSectionData* SectionData = dynamic->getSectionData();
  assert(SectionData && ".dynamic section has no MCSectionData");

  MemoryRegion* region = pOutput.memArea()->request(dynamic->offset(),
                                                    dynamic->size(),
                                                    true);

  llvm::ELF::Elf32_Dyn* buffer =
    reinterpret_cast<llvm::ELF::Elf32_Dyn*>(region->getBuffer());

  const llvm::ELF::Elf32_Dyn* DynEntry = 0;
  DynamicEntryFn emitFn;

  llvm::MCSectionData::iterator it = SectionData->getFragmentList().begin();
  llvm::MCSectionData::iterator ie = SectionData->getFragmentList().end();

  while (it != ie) {
    DynEntry = reinterpret_cast<const llvm::ELF::Elf32_Dyn*>(
                            llvm::cast<llvm::MCDataFragment>(
                            (*it)).getContents().str().begin());

    llvm::ELF::Elf32_Sword tag = DynEntry->d_tag;

    assert((0 <= tag && tag < dynamic_type_num) &&
            "Unknown dynamic section tags");

    // Standard dynamic section tags
    emitFn = emitDynamicEntry[tag];

    if (emitFn) {
      ELFDynObjFileFormat* FileFormat = getDynObjFileFormat();
      assert(!FileFormat && "ELFDynObjectFileFormat is NULL!");

      emitFn(buffer, FileFormat);
    }

    else
      llvm::report_fatal_error("Unsupported dynamic section tags");

    ++it;
  }
}

/// getSectionOrder
unsigned int GNULDBackend::getSectionOrder(const LDSection& pSectHdr) const
{
  // NULL section should be the "1st" section
  if (LDFileFormat::Null == pSectHdr.kind())
    return 0;

  // if the section is not ALLOC, lay it out until the last possible moment
  if (0 == (pSectHdr.flag() & llvm::ELF::SHF_ALLOC))
    return SHO_UNDEFINED;

  bool is_write = (pSectHdr.flag() & llvm::ELF::SHF_WRITE) != 0;
  bool is_exec = (pSectHdr.flag() & llvm::ELF::SHF_EXECINSTR) != 0;

  // TODO: need to take care other possible output sections
  switch (pSectHdr.kind()) {
    case LDFileFormat::Regular:
      if (is_exec) {
        if (pSectHdr.name() == ".init")
          return SHO_INIT;
        else if (pSectHdr.name() == ".fini")
          return SHO_FINI;
        else
          return SHO_TEXT;
      } else if (!is_write) {
        return SHO_RO;
      } else {
        if ((pSectHdr.type() & llvm::ELF::SHT_PREINIT_ARRAY) != 0 ||
            (pSectHdr.type() & llvm::ELF::SHT_INIT_ARRAY) != 0 ||
            (pSectHdr.type() & llvm::ELF::SHT_FINI_ARRAY) != 0 ||
            pSectHdr.name() == ".ctors" ||
            pSectHdr.name() == ".dtors")
          return SHO_RELRO;

        return SHO_DATA;
      }

    case LDFileFormat::BSS:
      return SHO_BSS;

    case LDFileFormat::NamePool:
      return SHO_NAMEPOOL;

    case LDFileFormat::Relocation:
      if (std::string::npos != pSectHdr.name().find("plt"))
        return SHO_REL_PLT;
      return SHO_RELOCATION;

    // get the order from target for target specific sections
    case LDFileFormat::Target:
      return getTargetSectionOrder(pSectHdr);

    // handle .interp and .dynamic
    case LDFileFormat::Note:
      if (!is_write)
        return SHO_INTERP;
      else
        return SHO_RELRO;

    case LDFileFormat::MetaData:
    case LDFileFormat::Debug:
    default:
      return SHO_UNDEFINED;
  }
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

