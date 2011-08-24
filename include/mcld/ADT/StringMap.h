/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef MCLD_STRING_MAP_H_
#define MCLD_STRING_MAP_H_
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>
#include <llvm/ADT/StringMap.h>
#include <mcld/Support/Path.h>

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


//=====================
// StringMapIterator
template <typename ValueTy, typename AllocatorTy>
class StringMapConstIterator
{
protected:
  typedef StringMap<ValueTy, AllocatorTy> MapTy;

  MapTy* map;
  int32_t idx;

public:
  StringMapConstIterator() {}
  explicit StringMapConstIterator(MapTy* map, int32_t idx = -1)
  : map(map), idx(idx) {
    if (idx == -1)  idx = map->m_Content.size();
  }

  bool operator==(const StringMapConstIterator &RHS) const { return idx == RHS.idx; }
  bool operator!=(const StringMapConstIterator &RHS) const { return !operator==(RHS); }

  typedef ValueTy value_type;

  const value_type& operator*() const {
    return map->m_Content[idx];
  }
  const value_type* operator->() const {
    return &map->m_Content[idx];
  }

  StringMapConstIterator& operator++() { ++idx; return *this; }
  StringMapConstIterator operator++(int) { StringMapConstIterator Tmp(*this); ++idx; return Tmp; }
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
  ~StringMap() {}

  typedef StringMapIterator<ValueTy, AllocatorTy> iterator;
  typedef StringMapConstIterator<ValueTy, AllocatorTy> const_iterator;

  iterator begin() {  return iterator(this, 0); }
  const_iterator begin() const {  return const_iterator(this, 0); }
  iterator end() {  return iterator(this); }
  const_iterator end() const {  return const_iterator(this); }

  bool empty() const { return m_Content.empty(); }
  void clear() { m_Map.clear();  m_Content.clear(); }

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
