//===- StringMap.h --------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_STRING_MAP_H_
#define MCLD_STRING_MAP_H_
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <assert.h>
#include "llvm/ADT/StringMap.h"
#include "mcld/Support/Path.h"

namespace mcld {

  template <typename ValueTy, typename AllocatorTy>
  class StringMap;

template <typename ValueTy>
class StringMapEntryCreator {
public:
  template <typename InitTy>
  static ValueTy create(const InitTy &InitVal) {
    return InitVal;
  }
};

template <typename ValueTy>
class StringMapEntryEraser {
public:
  static void erase(ValueTy &Val) {
    return;
  }
};

//=====================
// StringMapIterator
template <typename ValueTy, typename AllocatorTy>
class StringMapConstIterator
{
protected:
  typedef StringMap<ValueTy, AllocatorTy> MapTy;

  MapTy* f_Map;
  int32_t f_Idx;

public:
  typedef ValueTy value_type;

public:
  StringMapConstIterator()
  : f_Map(0), f_Idx(-1)
  { }

  StringMapConstIterator(const StringMapConstIterator& pCopy)
  : f_Map(pCopy.f_Map), f_Idx(pCopy.f_Idx)
  { }

  explicit StringMapConstIterator(MapTy* map, int32_t idx = -1)
  : f_Map(map), f_Idx(idx) {
    if (idx == -1)
      f_Idx = map->m_Content.size();
  }

  StringMapConstIterator& operator=(const StringMapConstIterator& pCopy) {
    f_Map = pCopy.f_Map;
    f_Idx = pCopy.f_Idx;
    return (*this);
  }

  bool operator==(const StringMapConstIterator &RHS) const
  { return f_Idx == RHS.f_Idx; }

  bool operator!=(const StringMapConstIterator &RHS) const
  { return !operator==(RHS); }

  const value_type& operator*() const {
    assert(0 != f_Map);
    return f_Map->m_Content[f_Idx];
  }

  const value_type* operator->() const {
    assert(0 != f_Map);
    return &f_Map->m_Content[f_Idx];
  }

  StringMapConstIterator& operator++() {
    ++f_Idx;
    return *this;
  }

  StringMapConstIterator operator++(int) {
    StringMapConstIterator Tmp(*this);
    ++f_Idx;
    return Tmp;
  }
};

template <typename ValueTy, typename AllocatorTy>
class StringMapIterator : public StringMapConstIterator<ValueTy, AllocatorTy>
{
  typedef typename StringMapConstIterator<ValueTy, AllocatorTy>::MapTy MapTy;
  typedef typename StringMapConstIterator<ValueTy, AllocatorTy>::value_type value_type;
public:
  StringMapIterator() {}

  explicit StringMapIterator(MapTy* map, int32_t idx = -1)
  : StringMapConstIterator<ValueTy, AllocatorTy>(map, idx) {}

  using StringMapConstIterator<ValueTy, AllocatorTy>::operator*;
  using StringMapConstIterator<ValueTy, AllocatorTy>::operator->;

  value_type& operator*() {
    return const_cast<value_type&>(
        const_cast<const StringMapIterator*>(this)->operator*());
  }
  value_type* operator->() {
    return const_cast<value_type*>(
        const_cast<const StringMapIterator*>(this)->operator->());
  }
};


//====================
// StringMap - Object adaptor of llvm::StringMap to make it
//           1. While insert, only find once, can get the iterator to the Value
//           2. Stable iterator
template <typename ValueTy, typename AllocatorTy = llvm::MallocAllocator>
class StringMap
{
  friend class StringMapConstIterator<ValueTy, AllocatorTy>;
  friend class StringMapIterator<ValueTy, AllocatorTy>;
  typedef size_t VectorIndexTy;

  llvm::StringMap<VectorIndexTy, AllocatorTy> m_Map;
  std::vector <ValueTy> m_Content;

public:
  StringMap() { m_Content.reserve(8); }
  ~StringMap();

  typedef StringMapIterator<ValueTy, AllocatorTy> iterator;
  typedef StringMapConstIterator<ValueTy, AllocatorTy> const_iterator;

  iterator begin()
  { return iterator(this, 0); }

  const_iterator begin() const
  { return const_iterator(this, 0); }

  iterator end()
  { return iterator(this); }

  const_iterator end() const
  { return const_iterator(this); }

  bool empty() const { return m_Content.empty(); }
  void clear() { m_Map.clear();  m_Content.clear(); }
  size_t size() const { return m_Content.size(); }

  template <typename InitTy>
  inline iterator GetOrCreateValue(llvm::StringRef Key, const InitTy &Val = ValueTy());
/*
  iterator find(llvm::StringRef Key); // TODO
  const_iterator find(llvm::StringRef Key) const; // TODO

  ValueTy lookup(llvm::StringRef Key) const;  // TODO
  ValueTy& operator[](llvm::StringRef Key); // TODO
*/
};

//==========================
// StringMap implementation
template <typename ValueTy, typename AllocatorTy>
StringMap<ValueTy, AllocatorTy>::~StringMap() {
  for (typename std::vector<ValueTy>::iterator i = m_Content.begin(), e = m_Content.end();
       i != e; ++i) {
    StringMapEntryEraser<ValueTy>::erase(*i);
  }
}

template <typename ValueTy, typename AllocatorTy>
template <typename InitTy>
typename StringMap<ValueTy, AllocatorTy>::iterator
StringMap<ValueTy, AllocatorTy>::GetOrCreateValue(llvm::StringRef Key, const InitTy &Value) {
  VectorIndexTy idx = m_Content.size();
  VectorIndexTy ret = m_Map.GetOrCreateValue(Key, idx).second;

  if (ret == idx) { // Create
    m_Content.push_back(StringMapEntryCreator<ValueTy>::create(Value));
  }

  return iterator(this, ret);
}

} // namespace mcld

#endif
