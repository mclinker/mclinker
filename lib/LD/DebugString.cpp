//===- DebugString.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/DebugString.h>
#include <mcld/LD/LDSection.h>
#include <mcld/LD/LDSymbol.h>
#include <mcld/LD/RelocData.h>
#include <mcld/LD/ResolveInfo.h>
#include <mcld/Fragment/Fragment.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/Fragment/Relocation.h>

#include <llvm/Support/Casting.h>

using namespace mcld;

static inline size_t string_length(const char* pStr)
{
  const char* p = pStr;
  size_t len = 0;
  for (; *p != 0; ++p)
    ++len;
  return len;
}

//==========================
// DebugString
void DebugString::processRelocs(LDSection& pSection)
{
  // if we've failed to processed a relocation section before, then the debug
  // string table is fail to merge. No need to process the others.
  if (!m_Success)
    return;

  // only need to proccess .debug_info
  if (strcmp(pSection.getLink()->name().c_str(), ".debug_info") != 0)
    return;

  ResolveInfo* d_str_sym = NULL;
  RelocData::iterator reloc, rend = pSection.getRelocData()->end();
  for (reloc = pSection.getRelocData()->begin(); reloc != rend; ++reloc) {
    Relocation* relocation = llvm::cast<Relocation>(reloc);
    ResolveInfo* info = relocation->symInfo();
    assert(info->outSymbol()->hasFragRef());
    // get the first relocation refer to the section .debug_str
    if (strcmp(".debug_str", info->name()) == 0) {
      d_str_sym = info;
      break;
    }
  }

  // return if no relocation refer to .debug_str
  if (d_str_sym == NULL)
    return;

  assert(d_str_sym->outSymbol()->hasFragRef());
  // section symbol should point to the first region fragment in the section
  // get the input .debut_str region
  llvm::StringRef d_str;
  if (d_str_sym->outSymbol()->fragRef()->frag()->getKind() == Fragment::Region) {
    RegionFragment* frag =
          llvm::cast<RegionFragment>(d_str_sym->outSymbol()->fragRef()->frag());
     d_str = frag->getRegion();
  }
  else {
    m_Success = false;
    return;
  }

  for (; reloc != rend; ++reloc) {
    Relocation* relocation = llvm::cast<Relocation>(reloc);
    ResolveInfo* info = relocation->symInfo();
    assert(info->outSymbol()->hasFragRef());
    // record the relocation refer to .debug_str, and also add the string to
    // merged string table
    if (info != d_str_sym)
      continue;

    // get the string offset
    // FIXME: Here we should ask backend to check the relocation type and get
    // the string offset
    uint32_t offset = relocation->target();
    const char* str = d_str.data() + offset;
    MergedStringTable::StringMapEntryTy& map_entry =
      m_StringTable.getOrCreateString(llvm::StringRef(str, string_length(str)));

    m_RelocStringList.push_back(EntryTy(*relocation, map_entry));
  }
}

void DebugString::applyOffset()
{
  // traverse the recorded relocations and set the offset
  entry_iterator it, end = m_RelocStringList.end();
  for (it = m_RelocStringList.begin(); it != end; ++it) {
    // get the string final offset
    size_t offset = m_StringTable.getOutputOffset((*it).second);
    // apply the offset to relocation target
    // FIXME: we should ask beckend to check the relocation type and set the
    // target data
    (*it).first.target() = offset;
  }
}

uint64_t DebugString::sizeStringTable()
{
  if (m_pSection != NULL) {
    m_pSection->setSize(m_StringTable.finalizeOffset());
    return m_pSection->size();
  }
  return 0;
}

void DebugString::emit(MemoryRegion& pRegion)
{
  return m_StringTable.emit(pRegion);
}

