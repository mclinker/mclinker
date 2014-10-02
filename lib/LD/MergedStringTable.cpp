//===- MergedStringTable.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/MergedStringTable.h>

namespace mcld {

MergedStringTable::StringMapEntryTy&
MergedStringTable::getOrCreateString(llvm::StringRef pString) {
  return m_StringMap.GetOrCreateValue(pString);
}

uint64_t MergedStringTable::finalizeOffset() {
  // trverse the string table and set the offset
  string_map_iterator it, end = m_StringMap.end();
  size_t offset = 0;
  for (it = m_StringMap.begin(); it != end; ++it) {
    it->setValue(offset);
    offset += it->getKey().size() + 1;
  }
  return offset;
}

void MergedStringTable::emit(MemoryRegion& pRegion) {
  char* ptr = reinterpret_cast<char*>(pRegion.begin());
  string_map_iterator it, end = m_StringMap.end();
  for (it = m_StringMap.begin(); it != end; ++it) {
    ::memcpy(ptr, it->getKey().data(), it->getKey().size());
    ptr += it->getKey().size() + 1;
  }
}

size_t MergedStringTable::getOutputOffset(
    MergedStringTable::StringMapEntryTy& pEntry) {
  return pEntry.getValue();
}

size_t MergedStringTable::getOutputOffset(llvm::StringRef pStr) {
  assert(m_StringMap.find(pStr) != m_StringMap.end());
  return m_StringMap[pStr];
}

}  // namespace mcld
