//===- GNULDBackend.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Target/GNULDBackend.h>

#include <string>
#include <cstring>
#include <cassert>

#include <llvm/Support/ELF.h>

#include <mcld/ADT/SizeTraits.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/Layout.h>
#include <mcld/LD/FillFragment.h>
#include <mcld/MC/MCLDInfo.h>
#include <mcld/MC/MCLDOutput.h>
#include <mcld/MC/InputTree.h>
#include <mcld/MC/SymbolCategory.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Support/MemoryArea.h>
#include <mcld/Support/MemoryRegion.h>
#include <mcld/Support/MsgHandling.h>
#include <mcld/Support/MemoryAreaFactory.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GNULDBackend
//===----------------------------------------------------------------------===//
GNULDBackend::GNULDBackend()
  : m_pArchiveReader(NULL),
    m_pObjectReader(NULL),
    m_pDynObjReader(NULL),
    m_pObjectWriter(NULL),
    m_pDynObjWriter(NULL),
    m_pExecWriter(NULL),
    m_pDynObjFileFormat(NULL),
    m_pExecFileFormat(NULL),
    m_ELFSegmentTable(9), // magic number
    m_pEhFrameHdr(NULL),
    f_pPreInitArrayStart(NULL),
    f_pPreInitArrayEnd(NULL),
    f_pInitArrayStart(NULL),
    f_pInitArrayEnd(NULL),
    f_pFiniArrayStart(NULL),
    f_pFiniArrayEnd(NULL),
    f_pStack(NULL),
    f_pExecutableStart(NULL),
    f_pEText(NULL),
    f_p_EText(NULL),
    f_p__EText(NULL),
    f_pEData(NULL),
    f_p_EData(NULL),
    f_pBSSStart(NULL),
    f_pEnd(NULL),
    f_p_End(NULL) {
  m_pSymIndexMap = new HashTableType(1024);
}

GNULDBackend::~GNULDBackend()
{
  if (NULL != m_pArchiveReader)
    delete m_pArchiveReader;
  if (NULL != m_pObjectReader)
    delete m_pObjectReader;
  if (NULL != m_pDynObjReader)
    delete m_pDynObjReader;
  if (NULL != m_pObjectWriter)
    delete m_pObjectWriter;
  if (NULL != m_pDynObjWriter)
    delete m_pDynObjWriter;
  if (NULL != m_pExecWriter)
    delete m_pExecWriter;
  if (NULL != m_pDynObjFileFormat)
    delete m_pDynObjFileFormat;
  if (NULL != m_pExecFileFormat)
    delete m_pExecFileFormat;
  if (NULL != m_pSymIndexMap)
    delete m_pSymIndexMap;
  if (NULL != m_pEhFrameHdr)
    delete m_pEhFrameHdr;
}

size_t GNULDBackend::sectionStartOffset() const
{
  // FIXME: use fixed offset, we need 10 segments by default
  return sizeof(llvm::ELF::Elf64_Ehdr)+10*sizeof(llvm::ELF::Elf64_Phdr);
}

uint64_t GNULDBackend::segmentStartAddr(const Output& pOutput,
                                        const MCLDInfo& pInfo) const
{
  // TODO: handle the user option: -TText=
  if (isOutputPIC(pOutput, pInfo))
    return 0x0;
  else
    return defaultTextSegmentAddr();
}

bool GNULDBackend::initArchiveReader(MCLinker& pLinker,
                                     MCLDInfo& pInfo,
                                     MemoryAreaFactory& pMemAreaFactory)
{
  if (NULL == m_pArchiveReader) {
    assert(NULL != m_pObjectReader);
    m_pArchiveReader = new GNUArchiveReader(pInfo,
                                            pMemAreaFactory,
                                            *m_pObjectReader);
  }
  return true;
}

bool GNULDBackend::initObjectReader(MCLinker& pLinker)
{
  if (NULL == m_pObjectReader)
    m_pObjectReader = new ELFObjectReader(*this, pLinker);
  return true;
}

bool GNULDBackend::initDynObjReader(MCLinker& pLinker)
{
  if (NULL == m_pDynObjReader)
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
  if (NULL == m_pDynObjWriter)
    m_pDynObjWriter = new ELFDynObjWriter(*this, pLinker);
  return true;
}

bool GNULDBackend::initExecWriter(MCLinker& pLinker)
{
  if (NULL == m_pExecWriter)
    m_pExecWriter = new ELFExecWriter(*this, pLinker);
  return true;
}

bool GNULDBackend::initExecSections(MCLinker& pMCLinker)
{
  if (NULL == m_pExecFileFormat)
    m_pExecFileFormat = new ELFExecFileFormat(*this);

  // initialize standard sections
  m_pExecFileFormat->initStdSections(pMCLinker);
  return true;
}

bool GNULDBackend::initDynObjSections(MCLinker& pMCLinker)
{
  if (NULL == m_pDynObjFileFormat)
    m_pDynObjFileFormat = new ELFDynObjFileFormat(*this);

  // initialize standard sections
  m_pDynObjFileFormat->initStdSections(pMCLinker);
  return true;
}

bool GNULDBackend::initStandardSymbols(MCLinker& pLinker, const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  // -----  section symbols  ----- //
  // .preinit_array
  FragmentRef* preinit_array = NULL;
  if (file_format->hasPreInitArray()) {
    preinit_array = pLinker.getLayout().getFragmentRef(
                   *(file_format->getPreInitArray().getSectionData()->begin()),
                   0x0);
  }
  f_pPreInitArrayStart =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__preinit_array_start",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             preinit_array, // FragRef
                                             ResolveInfo::Hidden);
  f_pPreInitArrayEnd =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__preinit_array_end",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Hidden);

  // .init_array
  FragmentRef* init_array = NULL;
  if (file_format->hasInitArray()) {
    init_array = pLinker.getLayout().getFragmentRef(
                      *(file_format->getInitArray().getSectionData()->begin()),
                      0x0);
  }

  f_pInitArrayStart =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__init_array_start",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             init_array, // FragRef
                                             ResolveInfo::Hidden);
  f_pInitArrayEnd =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__init_array_end",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             init_array, // FragRef
                                             ResolveInfo::Hidden);

  // .fini_array
  FragmentRef* fini_array = NULL;
  if (file_format->hasFiniArray()) {
    fini_array = pLinker.getLayout().getFragmentRef(
                     *(file_format->getFiniArray().getSectionData()->begin()),
                     0x0);
  }

  f_pFiniArrayStart =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__fini_array_start",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             fini_array, // FragRef
                                             ResolveInfo::Hidden);
  f_pFiniArrayEnd =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__fini_array_end",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             fini_array, // FragRef
                                             ResolveInfo::Hidden);

  // .stack
  FragmentRef* stack = NULL;
  if (file_format->hasStack()) {
    stack = pLinker.getLayout().getFragmentRef(
                          *(file_format->getStack().getSectionData()->begin()),
                          0x0);
  }
  f_pStack =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__stack",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Global,
                                             0x0, // size
                                             0x0, // value
                                             stack, // FragRef
                                             ResolveInfo::Hidden);

  // -----  segment symbols  ----- //
  f_pExecutableStart =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__executable_start",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);
  f_pEText =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("etext",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);
  f_p_EText =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("_etext",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);
  f_p__EText =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("__etext",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);
  f_pEData =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("edata",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);

  f_pEnd =
     pLinker.defineSymbol<MCLinker::AsRefered,
                          MCLinker::Resolve>("end",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);

  // _edata is defined forcefully.
  // @ref Google gold linker: defstd.cc: 186
  f_p_EData =
     pLinker.defineSymbol<MCLinker::Force,
                          MCLinker::Resolve>("_edata",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);

  // __bss_start is defined forcefully.
  // @ref Google gold linker: defstd.cc: 214
  f_pBSSStart =
     pLinker.defineSymbol<MCLinker::Force,
                          MCLinker::Resolve>("__bss_start",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);

  // _end is defined forcefully.
  // @ref Google gold linker: defstd.cc: 228
  f_p_End =
     pLinker.defineSymbol<MCLinker::Force,
                          MCLinker::Resolve>("_end",
                                             false, // isDyn
                                             ResolveInfo::NoType,
                                             ResolveInfo::Define,
                                             ResolveInfo::Absolute,
                                             0x0, // size
                                             0x0, // value
                                             NULL, // FragRef
                                             ResolveInfo::Default);

  return true;
}

bool
GNULDBackend::finalizeStandardSymbols(MCLinker& pLinker, const Output& pOutput)
{
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  // -----  section symbols  ----- //
  if (NULL != f_pPreInitArrayStart) {
    if (!f_pPreInitArrayStart->hasFragRef()) {
      f_pPreInitArrayStart->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pPreInitArrayStart->setValue(0x0);
    }
  }

  if (NULL != f_pPreInitArrayEnd) {
    if (f_pPreInitArrayEnd->hasFragRef()) {
      f_pPreInitArrayEnd->setValue(f_pPreInitArrayEnd->value() +
                                   file_format->getPreInitArray().size());
    }
    else {
      f_pPreInitArrayEnd->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pPreInitArrayEnd->setValue(0x0);
    }
  }

  if (NULL != f_pInitArrayStart) {
    if (!f_pInitArrayStart->hasFragRef()) {
      f_pInitArrayStart->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pInitArrayStart->setValue(0x0);
    }
  }

  if (NULL != f_pInitArrayEnd) {
    if (f_pInitArrayEnd->hasFragRef()) {
      f_pInitArrayEnd->setValue(f_pInitArrayEnd->value() +
                                file_format->getInitArray().size());
    }
    else {
      f_pInitArrayEnd->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pInitArrayEnd->setValue(0x0);
    }
  }

  if (NULL != f_pFiniArrayStart) {
    if (!f_pFiniArrayStart->hasFragRef()) {
      f_pFiniArrayStart->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pFiniArrayStart->setValue(0x0);
    }
  }

  if (NULL != f_pFiniArrayEnd) {
    if (f_pFiniArrayEnd->hasFragRef()) {
      f_pFiniArrayEnd->setValue(f_pFiniArrayEnd->value() +
                                file_format->getFiniArray().size());
    }
    else {
      f_pFiniArrayEnd->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pFiniArrayEnd->setValue(0x0);
    }
  }

  if (NULL != f_pStack) {
    if (!f_pStack->hasFragRef()) {
      f_pStack->resolveInfo()->setBinding(ResolveInfo::Absolute);
      f_pStack->setValue(0x0);
    }
  }

  // -----  segment symbols  ----- //
  if (NULL != f_pExecutableStart) {
    ELFSegment* exec_start = m_ELFSegmentTable.find(llvm::ELF::PT_LOAD, 0x0, 0x0);
    if (NULL != exec_start) {
      if (ResolveInfo::ThreadLocal != f_pExecutableStart->type()) {
        f_pExecutableStart->setValue(f_pExecutableStart->value() +
                                     exec_start->vaddr());
      }
    }
    else
      f_pExecutableStart->setValue(0x0);
  }

  if (NULL != f_pEText || NULL != f_p_EText || NULL !=f_p__EText) {
    ELFSegment* etext = m_ELFSegmentTable.find(llvm::ELF::PT_LOAD,
                                               llvm::ELF::PF_X,
                                               llvm::ELF::PF_W);
    if (NULL != etext) {
      if (NULL != f_pEText && ResolveInfo::ThreadLocal != f_pEText->type()) {
        f_pEText->setValue(f_pEText->value() +
                           etext->vaddr() +
                           etext->memsz());
      }
      if (NULL != f_p_EText && ResolveInfo::ThreadLocal != f_p_EText->type()) {
        f_p_EText->setValue(f_p_EText->value() +
                            etext->vaddr() +
                            etext->memsz());
      }
      if (NULL != f_p__EText && ResolveInfo::ThreadLocal != f_p__EText->type()) {
        f_p__EText->setValue(f_p__EText->value() +
                            etext->vaddr() +
                            etext->memsz());
      }
    }
    else {
      if (NULL != f_pEText)
        f_pEText->setValue(0x0);
      if (NULL != f_p_EText)
        f_p_EText->setValue(0x0);
      if (NULL != f_p__EText)
        f_p__EText->setValue(0x0);
    }
  }

  if (NULL != f_pEData || NULL != f_p_EData || NULL != f_pBSSStart ||
      NULL != f_pEnd || NULL != f_p_End) {
    ELFSegment* edata = m_ELFSegmentTable.find(llvm::ELF::PT_LOAD,
                                               llvm::ELF::PF_W,
                                               0x0);
    if (NULL != edata) {
      if (NULL != f_pEData && ResolveInfo::ThreadLocal != f_pEData->type()) {
        f_pEData->setValue(f_pEData->value() +
                            edata->vaddr() +
                            edata->filesz());
      }
      if (NULL != f_p_EData && ResolveInfo::ThreadLocal != f_p_EData->type()) {
        f_p_EData->setValue(f_p_EData->value() +
                            edata->vaddr() +
                            edata->filesz());
      }
      if (NULL != f_pBSSStart && ResolveInfo::ThreadLocal != f_pBSSStart->type()) {
        f_pBSSStart->setValue(f_pBSSStart->value() +
                              edata->vaddr() +
                              edata->filesz());
      }

      if (NULL != f_pEnd && ResolveInfo::ThreadLocal != f_pEnd->type()) {
        f_pEnd->setValue(f_pEnd->value() +
                         edata->vaddr() +
                         edata->memsz());
      }
      if (NULL != f_p_End && ResolveInfo::ThreadLocal != f_p_End->type()) {
        f_p_End->setValue(f_p_End->value() +
                          edata->vaddr() +
                          edata->memsz());
      }
    }
    else {
      if (NULL != f_pEData)
        f_pEData->setValue(0x0);
      if (NULL != f_p_EData)
        f_p_EData->setValue(0x0);
      if (NULL != f_pBSSStart)
        f_pBSSStart->setValue(0x0);

      if (NULL != f_pEnd)
        f_pEnd->setValue(0x0);
      if (NULL != f_p_End)
        f_p_End->setValue(0x0);
    }
  }

  return true;
}

GNUArchiveReader *GNULDBackend::getArchiveReader()
{
  assert(NULL != m_pArchiveReader);
  return m_pArchiveReader;
}

const GNUArchiveReader *GNULDBackend::getArchiveReader() const
{
  assert(NULL != m_pArchiveReader);
  return m_pArchiveReader;
}

ELFObjectReader *GNULDBackend::getObjectReader()
{
  assert(NULL != m_pObjectReader);
  return m_pObjectReader;
}

const ELFObjectReader *GNULDBackend::getObjectReader() const
{
  assert(NULL != m_pObjectReader);
  return m_pObjectReader;
}

ELFDynObjReader *GNULDBackend::getDynObjReader()
{
  assert(NULL != m_pDynObjReader);
  return m_pDynObjReader;
}

const ELFDynObjReader *GNULDBackend::getDynObjReader() const
{
  assert(NULL != m_pDynObjReader);
  return m_pDynObjReader;
}

ELFObjectWriter *GNULDBackend::getObjectWriter()
{
  // TODO
  return NULL;
}

const ELFObjectWriter *GNULDBackend::getObjectWriter() const
{
  // TODO
  return NULL;
}

ELFDynObjWriter *GNULDBackend::getDynObjWriter()
{
  assert(NULL != m_pDynObjWriter);
  return m_pDynObjWriter;
}

const ELFDynObjWriter *GNULDBackend::getDynObjWriter() const
{
  assert(NULL != m_pDynObjWriter);
  return m_pDynObjWriter;
}

ELFExecWriter *GNULDBackend::getExecWriter()
{
  assert(NULL != m_pExecWriter);
  return m_pExecWriter;
}

const ELFExecWriter *GNULDBackend::getExecWriter() const
{
  assert(NULL != m_pExecWriter);
  return m_pExecWriter;
}

ELFFileFormat* GNULDBackend::getOutputFormat(const Output& pOutput)
{
  switch (pOutput.type()) {
    case Output::DynObj:
      return getDynObjFileFormat();
    case Output::Exec:
      return getExecFileFormat();
    // FIXME: We do not support building .o now
    case Output::Object:
    default:
      fatal(diag::unrecognized_output_file) << pOutput.type();
      return NULL;
  }
}

const ELFFileFormat* GNULDBackend::getOutputFormat(const Output& pOutput) const
{
  switch (pOutput.type()) {
    case Output::DynObj:
      return getDynObjFileFormat();
    case Output::Exec:
      return getExecFileFormat();
    // FIXME: We do not support building .o now
    case Output::Object:
    default:
      fatal(diag::unrecognized_output_file) << pOutput.type();
      return NULL;
  }
}

ELFDynObjFileFormat* GNULDBackend::getDynObjFileFormat()
{
  assert(NULL != m_pDynObjFileFormat);
  return m_pDynObjFileFormat;
}

const ELFDynObjFileFormat* GNULDBackend::getDynObjFileFormat() const
{
  assert(NULL != m_pDynObjFileFormat);
  return m_pDynObjFileFormat;
}

ELFExecFileFormat* GNULDBackend::getExecFileFormat()
{
  assert(NULL != m_pExecFileFormat);
  return m_pExecFileFormat;
}

const ELFExecFileFormat* GNULDBackend::getExecFileFormat() const
{
  assert(NULL != m_pExecFileFormat);
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

  ELFFileFormat* file_format = getOutputFormat(pOutput);

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

  ELFFileFormat* file_format = getOutputFormat(pOutput);
  if (pOutput.type() == Output::Object) {
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
  ELFFileFormat* file_format = getOutputFormat(pOutput);

  bool sym_exist = false;
  HashTableType::entry_type* entry = 0;

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
  if (Output::DynObj == pOutput.type())
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

/// sizeInterp - compute the size of the .interp section
void GNULDBackend::sizeInterp(const Output& pOutput, const MCLDInfo& pLDInfo)
{
  assert(pOutput.type() == Output::Exec);

  const char* dyld_name;
  if (pLDInfo.options().hasDyld())
    dyld_name = pLDInfo.options().dyld().c_str();
  else
    dyld_name = dyld();

  LDSection& interp = getExecFileFormat()->getInterp();
  interp.setSize(std::strlen(dyld_name) + 1);
}

/// emitInterp - emit the .interp
void GNULDBackend::emitInterp(Output& pOutput, const MCLDInfo& pLDInfo)
{
  assert(pOutput.type() == Output::Exec &&
         getExecFileFormat()->hasInterp() &&
         pOutput.hasMemArea());

  const LDSection& interp = getExecFileFormat()->getInterp();
  MemoryRegion *region = pOutput.memArea()->request(
                                              interp.offset(), interp.size());
  const char* dyld_name;
  if (pLDInfo.options().hasDyld())
    dyld_name = pLDInfo.options().dyld().c_str();
  else
    dyld_name = dyld();

  std::memcpy(region->start(), dyld_name, interp.size());
}

/// getSectionOrder
unsigned int GNULDBackend::getSectionOrder(const Output& pOutput,
                                           const LDSection& pSectHdr,
                                           const MCLDInfo& pInfo) const
{
  // NULL section should be the "1st" section
  if (LDFileFormat::Null == pSectHdr.kind())
    return 0;

  // if the section is not ALLOC, lay it out until the last possible moment
  if (0 == (pSectHdr.flag() & llvm::ELF::SHF_ALLOC))
    return SHO_UNDEFINED;

  bool is_write = (pSectHdr.flag() & llvm::ELF::SHF_WRITE) != 0;
  bool is_exec = (pSectHdr.flag() & llvm::ELF::SHF_EXECINSTR) != 0;
  const ELFFileFormat* file_format = getOutputFormat(pOutput);

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
        if (pInfo.options().hasRelro()) {
          if (pSectHdr.type() == llvm::ELF::SHT_PREINIT_ARRAY ||
              pSectHdr.type() == llvm::ELF::SHT_INIT_ARRAY ||
              pSectHdr.type() == llvm::ELF::SHT_FINI_ARRAY ||
              &pSectHdr == &file_format->getCtors() ||
              &pSectHdr == &file_format->getDtors() ||
              &pSectHdr == &file_format->getJCR() ||
              0 == pSectHdr.name().compare(".data.rel.ro"))
            return SHO_RELRO;
          if (0 == pSectHdr.name().compare(".data.rel.ro.local"))
            return SHO_RELRO_LOCAL;
        }
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
      return getTargetSectionOrder(pOutput, pSectHdr, pInfo);

    // handle .interp
    case LDFileFormat::Note:
      return SHO_INTERP;

    case LDFileFormat::EhFrame:
    case LDFileFormat::EhFrameHdr:
    case LDFileFormat::GCCExceptTable:
      return SHO_EXCEPTION;

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

  uint32_t type = pSymbol.resolveInfo()->type();
  // if the IndirectFunc symbol (i.e., STT_GNU_IFUNC) is from dynobj, change
  // its type to Function
  if (type == ResolveInfo::IndirectFunc && pSymbol.isDyn())
    type = ResolveInfo::Function;
  return (type | (bind << 4));
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

  assert(pSymbol.hasFragRef() && "symbols must have fragment reference to get its index");
  return pLayout.getOutputLDSection(*pSymbol.fragRef()->frag())->index();
}

/// getSymbolIdx - called by emitRelocation to get the ouput symbol table index
size_t GNULDBackend::getSymbolIdx(LDSymbol* pSymbol) const
{
   HashTableType::iterator entry = m_pSymIndexMap->find(pSymbol);
   return entry.getEntry()->value();
}

/// allocateCommonSymbols - allocate common symbols in the corresponding
/// sections.
/// @refer Google gold linker: common.cc: 214
bool
GNULDBackend::allocateCommonSymbols(const MCLDInfo& pInfo, MCLinker& pLinker) const
{
  SymbolCategory& symbol_list = pLinker.getOutputSymbols();

  if (symbol_list.emptyCommons() && symbol_list.emptyLocals())
    return true;

  SymbolCategory::iterator com_sym, com_end;

  // FIXME: If the order of common symbols is defined, then sort common symbols
  // std::sort(com_sym, com_end, some kind of order);

  // get or create corresponding BSS LDSection
  LDSection* bss_sect = &pLinker.getOrCreateOutputSectHdr(".bss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC);

  LDSection* tbss_sect = &pLinker.getOrCreateOutputSectHdr(
                                   ".tbss",
                                   LDFileFormat::BSS,
                                   llvm::ELF::SHT_NOBITS,
                                   llvm::ELF::SHF_WRITE | llvm::ELF::SHF_ALLOC);

  assert(NULL != bss_sect && NULL !=tbss_sect);

  // get or create corresponding BSS SectionData
  SectionData& bss_sect_data = pLinker.getOrCreateSectData(*bss_sect);
  SectionData& tbss_sect_data = pLinker.getOrCreateSectData(*tbss_sect);

  // remember original BSS size
  uint64_t bss_offset  = bss_sect->size();
  uint64_t tbss_offset = tbss_sect->size();

  // allocate all local common symbols
  com_end = symbol_list.localEnd();

  for (com_sym = symbol_list.localBegin(); com_sym != com_end; ++com_sym) {
    if (ResolveInfo::Common == (*com_sym)->desc()) {
      // We have to reset the description of the symbol here. When doing
      // incremental linking, the output relocatable object may have common
      // symbols. Therefore, we can not treat common symbols as normal symbols
      // when emitting the regular name pools. We must change the symbols'
      // description here.
      (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
      Fragment* frag = new FillFragment(0x0, 1, (*com_sym)->size());
      (*com_sym)->setFragmentRef(new FragmentRef(*frag, 0));

      if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
        // allocate TLS common symbol in tbss section
        tbss_offset += pLinker.getLayout().appendFragment(*frag,
                                                          tbss_sect_data,
                                                          (*com_sym)->value());
      }
      else {
        bss_offset += pLinker.getLayout().appendFragment(*frag,
                                                         bss_sect_data,
                                                         (*com_sym)->value());
      }
    }
  }

  // allocate all global common symbols
  com_end = symbol_list.commonEnd();
  for (com_sym = symbol_list.commonBegin(); com_sym != com_end; ++com_sym) {
    // We have to reset the description of the symbol here. When doing
    // incremental linking, the output relocatable object may have common
    // symbols. Therefore, we can not treat common symbols as normal symbols
    // when emitting the regular name pools. We must change the symbols'
    // description here.
    (*com_sym)->resolveInfo()->setDesc(ResolveInfo::Define);
    Fragment* frag = new FillFragment(0x0, 1, (*com_sym)->size());
    (*com_sym)->setFragmentRef(new FragmentRef(*frag, 0));

    if (ResolveInfo::ThreadLocal == (*com_sym)->type()) {
      // allocate TLS common symbol in tbss section
      tbss_offset += pLinker.getLayout().appendFragment(*frag,
                                                        tbss_sect_data,
                                                        (*com_sym)->value());
    }
    else {
      bss_offset += pLinker.getLayout().appendFragment(*frag,
                                                       bss_sect_data,
                                                       (*com_sym)->value());
    }
  }

  bss_sect->setSize(bss_offset);
  tbss_sect->setSize(tbss_offset);
  symbol_list.changeCommonsToGlobal();
  return true;
}


/// createProgramHdrs - base on output sections to create the program headers
void GNULDBackend::createProgramHdrs(Output& pOutput, const MCLDInfo& pInfo)
{
  assert(pOutput.hasContext());
  ELFFileFormat *file_format = getOutputFormat(pOutput);

  // make PT_PHDR
  m_ELFSegmentTable.produce(llvm::ELF::PT_PHDR);

  // make PT_INTERP
  if (file_format->hasInterp()) {
    ELFSegment* interp_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_INTERP);
    interp_seg->addSection(&file_format->getInterp());
  }

  // FIXME: Should we consider -z relro here?
  if (pInfo.options().hasRelro()) {
    // if -z relro is given, we need to adjust sections' offset again, and let
    // PT_GNU_RELRO end on a common page boundary
    LDContext::SectionTable& sect_table = pOutput.context()->getSectionTable();

    size_t idx;
    for (idx = 0; idx < pOutput.context()->numOfSections(); ++idx) {
      // find the first non-relro section
      if (getSectionOrder(pOutput, *sect_table[idx], pInfo) > SHO_RELRO_LAST) {
        break;
      }
    }

    // align the first non-relro section to page boundary
    uint64_t offset = sect_table[idx]->offset();
    alignAddress(offset, commonPageSize(pInfo));
    sect_table[idx]->setOffset(offset);

    // set up remaining section's offset
    for (++idx; idx < pOutput.context()->numOfSections(); ++idx) {
      uint64_t offset;
      size_t prev_idx = idx - 1;
      if (LDFileFormat::BSS == sect_table[prev_idx]->kind())
        offset = sect_table[prev_idx]->offset();
      else
        offset = sect_table[prev_idx]->offset() + sect_table[prev_idx]->size();

      alignAddress(offset, sect_table[idx]->align());
      sect_table[idx]->setOffset(offset);
    }
  } // relro

  uint32_t cur_seg_flag, prev_seg_flag = getSegmentFlag(0);
  uint64_t padding = 0;
  ELFSegment* load_seg = NULL;
  // make possible PT_LOAD segments
  LDContext::sect_iterator sect, sect_end = pOutput.context()->sectEnd();
  for (sect = pOutput.context()->sectBegin(); sect != sect_end; ++sect) {

    if (0 == ((*sect)->flag() & llvm::ELF::SHF_ALLOC) &&
        LDFileFormat::Null != (*sect)->kind())
      continue;

    // FIXME: Now only separate writable and non-writable PT_LOAD
    cur_seg_flag = getSegmentFlag((*sect)->flag());
    if ((prev_seg_flag & llvm::ELF::PF_W) ^ (cur_seg_flag & llvm::ELF::PF_W) ||
         LDFileFormat::Null == (*sect)->kind()) {
      // create new PT_LOAD segment
      load_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_LOAD);
      load_seg->setAlign(abiPageSize(pInfo));

      // check if this segment needs padding
      padding = 0;
      if (((*sect)->offset() & (abiPageSize(pInfo) - 1)) != 0)
        padding = abiPageSize(pInfo);
    }

    assert(NULL != load_seg);
    load_seg->addSection((*sect));
    if (cur_seg_flag != prev_seg_flag)
      load_seg->updateFlag(cur_seg_flag);

    if (LDFileFormat::Null != (*sect)->kind())
      (*sect)->setAddr(segmentStartAddr(pOutput, pInfo) +
                       (*sect)->offset() +
                       padding);

    prev_seg_flag = cur_seg_flag;
  }

  // make PT_DYNAMIC
  if (file_format->hasDynamic()) {
    ELFSegment* dyn_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_DYNAMIC,
                                                    llvm::ELF::PF_R |
                                                    llvm::ELF::PF_W);
    dyn_seg->addSection(&file_format->getDynamic());
  }

  if (pInfo.options().hasRelro()) {
    // make PT_GNU_RELRO
    ELFSegment* relro_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_GNU_RELRO);
    for (LDContext::sect_iterator sect = pOutput.context()->sectBegin();
         sect != pOutput.context()->sectEnd(); ++sect) {
      unsigned int order = getSectionOrder(pOutput, **sect, pInfo);
      if (SHO_RELRO_LOCAL == order ||
          SHO_RELRO == order ||
          SHO_RELRO_LAST == order) {
        relro_seg->addSection(*sect);
      }
    }
  }

  // make PT_GNU_EH_FRAME
  if (file_format->hasEhFrameHdr()) {
    ELFSegment* eh_seg = m_ELFSegmentTable.produce(llvm::ELF::PT_GNU_EH_FRAME);
    eh_seg->addSection(&file_format->getEhFrameHdr());
  }
}

/// setupProgramHdrs - set up the attributes of segments
void GNULDBackend:: setupProgramHdrs(const Output& pOutput, const MCLDInfo& pInfo)
{
  // update segment info
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
      segment.setVaddr(segmentStartAddr(pOutput, pInfo) + offset);
      segment.setPaddr(segment.vaddr());
      segment.setFilesz(numOfSegments() * phdr_size);
      segment.setMemsz(numOfSegments() * phdr_size);
      segment.setAlign(bitclass() / 8);
      continue;
    }

    // bypass if there is no section in this segment (e.g., PT_GNU_STACK)
    if (segment.numOfSections() == 0)
      continue;

    segment.setOffset(segment.getFirstSection()->offset());
    if (llvm::ELF::PT_LOAD == segment.type() &&
        LDFileFormat::Null == segment.getFirstSection()->kind())
      segment.setVaddr(segmentStartAddr(pOutput, pInfo));
    else
      segment.setVaddr(segment.getFirstSection()->addr());
    segment.setPaddr(segment.vaddr());

    const LDSection* last_sect = segment.getLastSection();
    assert(NULL != last_sect);
    uint64_t file_size = last_sect->offset() - segment.offset();
    if (LDFileFormat::BSS != last_sect->kind())
      file_size += last_sect->size();
    segment.setFilesz(file_size);

    segment.setMemsz(last_sect->addr() - segment.vaddr() + last_sect->size());
  }
}

/// createGNUStackInfo - create an output GNU stack section or segment if needed
/// @ref gold linker: layout.cc:2608
void GNULDBackend::createGNUStackInfo(const Output& pOutput,
                                      const MCLDInfo& pInfo,
                                      MCLinker& pLinker)
{
  uint32_t flag = 0x0;
  if (pInfo.options().hasStackSet()) {
    // 1. check the command line option (-z execstack or -z noexecstack)
    if (pInfo.options().hasExecStack())
      flag = llvm::ELF::SHF_EXECINSTR;
  } else {
    // 2. check the stack info from the input objects
    size_t object_count = 0, stack_note_count = 0;
    mcld::InputTree::const_bfs_iterator input, inEnd = pInfo.inputs().bfs_end();
    for (input=pInfo.inputs().bfs_begin(); input!=inEnd; ++input) {
      if ((*input)->type() == Input::Object) {
        ++object_count;
        const LDSection* sect = (*input)->context()->getSection(
                                                             ".note.GNU-stack");
        if (NULL != sect) {
          ++stack_note_count;
          // 2.1 found a stack note that is set as executable
          if (0 != (llvm::ELF::SHF_EXECINSTR & sect->flag())) {
            flag = llvm::ELF::SHF_EXECINSTR;
            break;
          }
        }
      }
    }

    // 2.2 there are no stack note sections in all input objects
    if (0 == stack_note_count)
      return;

    // 2.3 a special case. Use the target default to decide if the stack should
    //     be executable
    if (llvm::ELF::SHF_EXECINSTR != flag && object_count != stack_note_count)
      if (isDefaultExecStack())
        flag = llvm::ELF::SHF_EXECINSTR;
  }

  if (pOutput.type() != Output::Object)
    m_ELFSegmentTable.produce(llvm::ELF::PT_GNU_STACK,
                              llvm::ELF::PF_R |
                              llvm::ELF::PF_W |
                              getSegmentFlag(flag));
  else
    pLinker.getOrCreateOutputSectHdr(".note.GNU-stack",
                                     LDFileFormat::Note,
                                     llvm::ELF::SHT_PROGBITS,
                                     flag);
}

/// preLayout - Backend can do any needed modification before layout
void GNULDBackend::preLayout(const Output& pOutput,
                             const MCLDInfo& pLDInfo,
                             MCLinker& pLinker)
{
  // prelayout target first
  doPreLayout(pOutput, pLDInfo, pLinker);

  if (pLDInfo.options().hasEhFrameHdr()) {
    // init EhFrameHdr and size the output section
    ELFFileFormat* format = getOutputFormat(pOutput);
    assert(NULL != getEhFrame());
    m_pEhFrameHdr = new EhFrameHdr(*getEhFrame(),
                                   format->getEhFrame(),
                                   format->getEhFrameHdr());
    m_pEhFrameHdr->sizeOutput();
  }
}

/// postLayout - Backend can do any needed modification after layout
void GNULDBackend::postLayout(const Output& pOutput,
                              const MCLDInfo& pInfo,
                              MCLinker& pLinker)
{
  // 1. emit program headers
  if (pOutput.type() != Output::Object) {
    // 1.1 create program headers
    createProgramHdrs(pLinker.getLDInfo().output(), pInfo);
  }

  // 1.2 create special GNU Stack note section or segment
  createGNUStackInfo(pOutput, pInfo, pLinker);

  if (pOutput.type() != Output::Object) {
    // 1.3 set up the attributes of program headers
    setupProgramHdrs(pOutput, pInfo);
  }

  // 2. target specific post layout
  doPostLayout(pOutput, pInfo, pLinker);
}

void GNULDBackend::postProcessing(const Output& pOutput,
                                  const MCLDInfo& pInfo,
                                  MCLinker& pLinker)
{
  if (pInfo.options().hasEhFrameHdr()) {
    // emit eh_frame_hdr
    if (bitclass() == 32)
      m_pEhFrameHdr->emitOutput<32>(pLinker.getLDInfo().output(),
                                    pLinker);
  }
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
  if (Output::DynObj == pOutput.type() || Output::Exec == pOutput.type())
    if (pSymbol.resolveInfo()->visibility() == ResolveInfo::Default ||
        pSymbol.resolveInfo()->visibility() == ResolveInfo::Protected)
      return true;
  return false;
}

/// commonPageSize - the common page size of the target machine.
/// @ref gold linker: target.h:135
uint64_t GNULDBackend::commonPageSize(const MCLDInfo& pInfo) const
{
  if (pInfo.options().commPageSize() > 0)
    return std::min(pInfo.options().commPageSize(), abiPageSize(pInfo));
  else
    return std::min(static_cast<uint64_t>(0x1000), abiPageSize(pInfo));
}

/// abiPageSize - the abi page size of the target machine.
/// @ref gold linker: target.h:125
uint64_t GNULDBackend::abiPageSize(const MCLDInfo& pInfo) const
{
  if (pInfo.options().maxPageSize() > 0)
    return pInfo.options().maxPageSize();
  else
    return static_cast<uint64_t>(0x1000);
}

/// isOutputPIC - return whether the output is position-independent
bool GNULDBackend::isOutputPIC(const Output& pOutput,
                               const MCLDInfo& pInfo) const
{
  if (Output::DynObj == pOutput.type() || pInfo.options().isPIE())
    return true;
  return false;
}

/// isStaticLink - return whether we're doing static link
bool GNULDBackend::isStaticLink(const Output& pOutput,
                                const MCLDInfo& pInfo) const
{
  InputTree::const_iterator it = pInfo.inputs().begin();
  if (!isOutputPIC(pOutput, pInfo) && (*it)->attribute()->isStatic())
    return true;
  return false;
}

/// isSymbolPreemtible - whether the symbol can be preemted by other
/// link unit
/// @ref Google gold linker, symtab.h:551
bool GNULDBackend::isSymbolPreemptible(const ResolveInfo& pSym,
                                       const MCLDInfo& pLDInfo,
                                       const Output& pOutput) const
{
  if (pSym.other() != ResolveInfo::Default)
    return false;

  if (Output::DynObj != pOutput.type())
    return false;

  if (pLDInfo.options().Bsymbolic())
    return false;

  return true;
}

/// symbolNeedsPLT - return whether the symbol needs a PLT entry
/// @ref Google gold linker, symtab.h:596
bool GNULDBackend::symbolNeedsPLT(const ResolveInfo& pSym,
                                  const MCLDInfo& pLDInfo,
                                  const Output& pOutput) const
{
  if (pSym.isUndef() && !pSym.isDyn() && pOutput.type() != Output::DynObj)
    return false;

  // An IndirectFunc symbol (i.e., STT_GNU_IFUNC) always needs a plt entry
  if (pSym.type() == ResolveInfo::IndirectFunc)
    return true;

  if (pSym.type() != ResolveInfo::Function)
    return false;

  if (isStaticLink(pOutput, pLDInfo) || pLDInfo.options().isPIE())
    return false;

  return (pSym.isDyn() ||
          pSym.isUndef() ||
          isSymbolPreemptible(pSym, pLDInfo, pOutput));
}

/// symbolNeedsDynRel - return whether the symbol needs a dynamic relocation
/// @ref Google gold linker, symtab.h:645
bool GNULDBackend::symbolNeedsDynRel(const ResolveInfo& pSym,
                                     bool pSymHasPLT,
                                     const MCLDInfo& pLDInfo,
                                     const Output& pOutput,
                                     bool isAbsReloc) const
{
  // an undefined reference in the executables should be statically
  // resolved to 0 and no need a dynamic relocation
  if (pSym.isUndef() && !pSym.isDyn() && (Output::Exec == pOutput.type()))
    return false;
  if (pSym.isAbsolute())
    return false;
  if (isOutputPIC(pOutput, pLDInfo) && isAbsReloc)
    return true;
  if (pSymHasPLT && ResolveInfo::Function == pSym.type())
    return false;
  if (!isOutputPIC(pOutput, pLDInfo) && pSymHasPLT)
    return false;
  if (pSym.isDyn() || pSym.isUndef() ||
      isSymbolPreemptible(pSym, pLDInfo, pOutput))
    return true;

  return false;
}

/// symbolNeedsCopyReloc - return whether the symbol needs a copy relocation
bool GNULDBackend::symbolNeedsCopyReloc(const Layout& pLayout,
                                        const Relocation& pReloc,
                                        const ResolveInfo& pSym,
                                        const MCLDInfo& pLDInfo,
                                        const Output& pOutput) const
{
  // only the reference from dynamic executable to non-function symbol in
  // the dynamic objects may need copy relocation
  if (isOutputPIC(pOutput, pLDInfo) ||
      !pSym.isDyn() ||
      pSym.type() == ResolveInfo::Function ||
      pSym.size() == 0)
    return false;

  // check if the option -z nocopyreloc is given
  if (pLDInfo.options().hasNoCopyReloc())
    return false;

  // TODO: Is this check necessary?
  // if relocation target place is readonly, a copy relocation is needed
  if ((pLayout.getOutputLDSection(*pReloc.targetRef().frag())->flag() &
      llvm::ELF::SHF_WRITE) == 0)
    return true;

  return false;
}

