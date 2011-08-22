/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef MCLD_STRING_MAP_H_
#define MCLD_STRING_MAP_H_
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/StringMap.h>
#include <mcld/Support/Path.h>

using mcld::sys::fs::Path;

namespace llvm {

/// This file is a partial specialization of llvm::StringMap.
/// We need more suitable interface when using our mcld::Path
template<typename AllocatorTy>
class StringMap<Path*, AllocatorTy> : public StringMapImpl {
  typedef Path* ValueTy;
  typedef StringMapEntry<ValueTy> MapEntryTy;

  AllocatorTy Allocator;
public:
  StringMap() : StringMapImpl(static_cast<unsigned>(sizeof(MapEntryTy))) {}
  explicit StringMap(unsigned InitialSize)
    : StringMapImpl(InitialSize, static_cast<unsigned>(sizeof(MapEntryTy))) {}

  explicit StringMap(AllocatorTy A)
    : StringMapImpl(static_cast<unsigned>(sizeof(MapEntryTy))), Allocator(A) {}

  explicit StringMap(const StringMap &RHS)
    : StringMapImpl(static_cast<unsigned>(sizeof(MapEntryTy))) {
    assert(RHS.empty() &&
           "Copy ctor from non-empty stringmap not implemented yet!");
    (void)RHS;
  }
  void operator=(const StringMap &RHS) {
    assert(RHS.empty() &&
           "assignment from non-empty stringmap not implemented yet!");
    (void)RHS;
    clear();
  }

  typedef typename ReferenceAdder<AllocatorTy>::result AllocatorRefTy;
  typedef typename ReferenceAdder<const AllocatorTy>::result AllocatorCRefTy;
  AllocatorRefTy getAllocator() { return Allocator; }
  AllocatorCRefTy getAllocator() const { return Allocator; }

  typedef const char* key_type;
  typedef ValueTy mapped_type;
  typedef StringMapEntry<ValueTy> value_type;
  typedef size_t size_type;

  typedef StringMapConstIterator<ValueTy> const_iterator;
  typedef StringMapIterator<ValueTy> iterator;

  iterator begin() {
    return iterator(TheTable, NumBuckets == 0);
  }
  iterator end() {
    return iterator(TheTable+NumBuckets, true);
  }
  const_iterator begin() const {
    return const_iterator(TheTable, NumBuckets == 0);
  }
  const_iterator end() const {
    return const_iterator(TheTable+NumBuckets, true);
  }

  iterator find(StringRef Key) {
    int Bucket = FindKey(Key);
    if (Bucket == -1) return end();
    return iterator(TheTable+Bucket);
  }

  const_iterator find(StringRef Key) const {
    int Bucket = FindKey(Key);
    if (Bucket == -1) return end();
    return const_iterator(TheTable+Bucket);
  }

   /// lookup - Return the entry for the specified key, or a default
  /// constructed value if no such entry exists.
  ValueTy lookup(StringRef Key) const {
    const_iterator it = find(Key);
    if (it != end())
      return it->second;
    return ValueTy();
  }

  ValueTy &operator[](StringRef Key) {
    return (*GetOrCreateValue(Key)).getValue();
  }

  size_type count(StringRef Key) const {
    return find(Key) == end() ? 0 : 1;
  }

  /// insert - Insert the specified key/value pair into the map.  If the key
  /// already exists in the map, return false and ignore the request, otherwise
  /// insert it and return true.
  bool insert(MapEntryTy *KeyValue) {
    unsigned BucketNo = LookupBucketFor(KeyValue->getKey());
    ItemBucket &Bucket = TheTable[BucketNo];
    if (Bucket.Item && Bucket.Item != getTombstoneVal())
      return false;  // Already exists in map.

    if (Bucket.Item == getTombstoneVal())
      --NumTombstones;
    Bucket.Item = KeyValue;
    ++NumItems;
    assert(NumItems + NumTombstones <= NumBuckets);

    RehashTable();
    return true;
  }

  // clear - Empties out the StringMap
  void clear() {
    if (empty()) return;

    // Zap all values, resetting the keys back to non-present (not tombstone),
    // which is safe because we're removing all elements.
    for (ItemBucket *I = TheTable, *E = TheTable+NumBuckets; I != E; ++I) {
      if (I->Item && I->Item != getTombstoneVal()) {
        static_cast<MapEntryTy*>(I->Item)->Destroy(Allocator);
        I->Item = 0;
      }
    }

    NumItems = 0;
    NumTombstones = 0;
  }

  template <typename InitTy>
  iterator GetOrCreateValue(StringRef Key, InitTy Val) {
    unsigned BucketNo = LookupBucketFor(Key);
    ItemBucket &Bucket = TheTable[BucketNo];
    if (Bucket.Item && Bucket.Item != getTombstoneVal())
      return iterator(&Bucket);

    MapEntryTy *NewItem =
      MapEntryTy::Create(Key.begin(), Key.end(), Allocator, Val);

    if (Bucket.Item == getTombstoneVal())
      --NumTombstones;
    ++NumItems;
    assert(NumItems + NumTombstones <= NumBuckets);

    // Fill in the bucket for the hash table.  The FullHashValue was already
    // filled in by LookupBucketFor.
    Bucket.Item = NewItem;

    RehashTable();
    return iterator(&Bucket);
  }

  iterator GetOrCreateValue(StringRef Key) {
    return GetOrCreateValue(Key, ValueTy());
  }

  /// remove - Remove the specified key/value pair from the map, but do not
  /// erase it.  This aborts if the key is not in the map.
  void remove(MapEntryTy *KeyValue) {
    RemoveKey(KeyValue);
  }

  void erase(iterator I) {
    MapEntryTy &V = *I;
    remove(&V);
    V.Destroy(Allocator);
  }

  bool erase(StringRef Key) {
    iterator I = find(Key);
    if (I == end()) return false;
    erase(I);
    return true;
  }

  ~StringMap() {
    clear();
    free(TheTable);
  }
};

}

#endif
