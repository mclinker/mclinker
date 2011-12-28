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
  size_t strtab = 1;
  size_t dynstr = 1;
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
      file_format->getDynSymTab().setSize(dynsym);
      file_format->getDynStrTab().setSize(dynstr);
      file_format->getHashTab().setSize(hash);
    }
    /* fall through */
    case Output::Object: {
      file_format->getSymTab().setSize(symtab);
      file_format->getStrTab().setSize(strtab);
      break;
    }
  }
}

/// emitRegNamePools - emit regular name pools - .symtab, .strtab
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
uint64_t GNULDBackend::emitRegNamePools(Output& pOutput,
                                        const MCLDInfo& pLDInfo) const
{
  // write out data
  return 0;
}

/// emitNamePools - emit dynamic name pools - .dyntab, .dynstr, .hash
///
/// the size of these tables should be computed before layout
/// layout should computes the start offset of these tables
uint64_t GNULDBackend::emitDynNamePools(Output& pOutput,
                                        const MCLDInfo& pLDInfo) const
{
  // TODO
  return 0;
}

unsigned int GNULDBackend::getSectionOrder(const LDSection& pSectHdr) const
{
  if (LDFileFormat::Target == pSectHdr.kind())
    return getTargetSectionOrder(pSectHdr);
  // TODO
  return ~(0U);
}

