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
#include <mcld/LD/SectionData.h>
#include <mcld/Fragment/Fragment.h>
#include <mcld/Fragment/RegionFragment.h>
#include <mcld/Fragment/Relocation.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/LD/Relocator.h>

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
void DebugString::processRelocs(LDSection& pSection,
                                const TargetLDBackend& pBackend)
{
  // if we've failed to processed a relocation section before, then the debug
  // string table is fail to merge. No need to process the others.
  if (!m_Success)
    return;

  // only need to proccess .debug_info
  if (strcmp(pSection.getLink()->name().c_str(), ".debug_info") != 0)
    return;

  // find the relocation against debug_str section. Add the referenced debug
  // string into merged string table and record the relocation
  ResolveInfo* d_str_sym = NULL;
  llvm::StringRef d_str;
  RelocData::iterator reloc, rend = pSection.getRelocData()->end();
  for (reloc = pSection.getRelocData()->begin(); reloc != rend; ++reloc) {
    Relocation* relocation = llvm::cast<Relocation>(reloc);
    ResolveInfo* info = relocation->symInfo();
    if (!info->outSymbol()->hasFragRef())
      continue;

    if (info != d_str_sym) {
      if (info->outSymbol()->fragRef()->frag()->getParent()->getSection().kind() !=
          LDFileFormat::DebugString)
        continue;
      // in most of the cases, the relocation will refer to the same symbol,
      // which is the section symbol of .debug_str. Record the symbol and the
      // fragment region to speed up comparison
      d_str_sym = info;
      // the symbol should point to the first region fragment in the debug
      // string section, get the input .debut_str region
      if (info->outSymbol()->fragRef()->frag()->getKind() == Fragment::Region) {
        RegionFragment* frag =
            llvm::cast<RegionFragment>(info->outSymbol()->fragRef()->frag());
        d_str = frag->getRegion();
      }
      else {
        m_Success = false;
        return;
      }
    }

    // record the relocation refer to .debug_str, and also add the string to
    // merged string table
    uint32_t offset = 0;
    if (!pBackend.getRelocator()->getDebugStringOffset(*relocation, offset)) {
      m_Success = false;
      return;
    }
    // get the debug string
    const char* str = d_str.data() + offset;
    MergedStringTable::StringMapEntryTy& map_entry =
      m_StringTable.getOrCreateString(llvm::StringRef(str, string_length(str)));

    m_RelocStringList.push_back(EntryTy(*relocation, map_entry));
  }
}

void DebugString::applyOffset(TargetLDBackend& pBackend)
{
  // traverse the recorded relocations and set the offset
  entry_iterator it, end = m_RelocStringList.end();
  for (it = m_RelocStringList.begin(); it != end; ++it) {
    // get the string final offset
    size_t offset = m_StringTable.getOutputOffset((*it).second);
    // apply the offset to relocation target
    pBackend.getRelocator()->applyDebugStringOffset((*it).first, offset);
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

