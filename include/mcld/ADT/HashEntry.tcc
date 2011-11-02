//===- HashEntry.tcc ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
// internal non-member functions
inline static unsigned int compute_bucket_count(unsigned int pNumOfBuckets)
{
  static const unsigned int bucket_size[] =
  {
    1, 3, 17, 37, 67, 97, 131, 197, 263, 521, 1031, 2053, 4099, 8209, 12289,
    16411, 24593, 32771, 49157, 65537, 98317, 131101, 196613, 262147, 393241
  };

  const int buckets_count = sizeof(bucket_size) / sizeof(bucket_size[0]);
  unsigned int idx = 0;
  do {
    if (pNumOfBuckets < bucket_size[idx])
      return bucket_size[idx];
    ++idx;
  } while( idx < buckets_count );

  return (pNumOfBuckets+131101); // rare case. increase constantly
}

//===--------------------------------------------------------------------===//
// template implementation of HashEntry
template<typename DataType>
HashEntry<DataType>::HashEntry()
  : m_StrLength(0) {
  m_String[0] = '\0';
}

template<typename DataType>
HashEntry<DataType>::~HashEntry()
{
}

template<typename DataType>
void HashEntry<DataType>::setKey(llvm::StringRef pKey)
{
  m_StrLength = pKey.size();
  std::memcpy(m_String, pKey.data(), m_StrLength+1);
  m_String[m_StrLength] = '\0';
}

template<typename DataType>
void HashEntry<DataType>::setKey(const char* pKey, unsigned int pLength)
{
  m_StrLength = pLength;
  std::memcpy(m_String, pKey, m_StrLength+1);
  m_String[m_StrLength] = '\0';
}

template<typename DataType>
llvm::StringRef HashEntry<DataType>::getKey() const
{
  return llvm::StringRef(m_String, m_StrLength);
}

template<typename DataType>
bool HashEntry<DataType>::equalKey(const char* pString) const
{
  return (0 == std::strcmp(m_String, pString));
}

template<typename DataType>
bool HashEntry<DataType>::equalKey(const char* pString, size_t pLength)
{
  if (m_StrLength == pLength)
    return (0 == std::strcmp(m_String, pString));
  return false;
}

template<typename DataType>
bool HashEntry<DataType>::equalKey(const std::string& pString) const
{
  if (m_StrLength == pString.size())
    return (0 == std::strcmp(m_String, pString.c_str()));
  return false;
}

template<typename DataType>
bool HashEntry<DataType>::equalKey(llvm::StringRef pString) const
{
  if (m_StrLength == pString.size())
    return (0 == std::strcmp(m_String, pString.data()));
  return false;
}

template<typename DataType>
llvm::StringRef HashEntry<DataType>::toStringRef() const
{
  return llvm::StringRef(m_String, m_StrLength);
}

template<typename DataType>
const char* HashEntry<DataType>::toCString() const
{
  return m_String;
}

template<typename DataType>
std::string HashEntry<DataType>::toString() const
{
  return std::string(m_String, m_StrLength);
}

//===--------------------------------------------------------------------===//
// template implementation of HashBucket
template<typename DataType>
typename HashBucket<DataType>::entry_type*
HashBucket<DataType>::getEmptyBucket()
{
  static entry_type* empty_bucket = reinterpret_cast<entry_type*>(0x0);
  return empty_bucket;
}

template<typename DataType>
typename HashBucket<DataType>::entry_type*
HashBucket<DataType>::getTombstone()
{
  static entry_type* tombstone = reinterpret_cast<entry_type*>(0x1);
  return tombstone;
}

//===--------------------------------------------------------------------===//
// template implementation of HashTableImpl
template<typename HashEntryTy,
         typename HashFunctionTy>
HashTableImpl<HashEntryTy, HashFunctionTy>::HashTableImpl()
  : m_Buckets(0), m_NumOfBuckets(0), m_NumOfItems(0), m_NumOfTombstones(0) {
}

template<typename HashEntryTy,
         typename HashFunctionTy>
HashTableImpl<HashEntryTy, HashFunctionTy>::HashTableImpl(unsigned int pInitSize)
{
  if (pInitSize) {
    init(pInitSize);
    return;
  }

  m_Buckets = 0;
  m_NumOfBuckets = 0;
  m_NumOfItems = 0;
  m_NumOfTombstones = 0;
}

template<typename HashEntryTy,
         typename HashFunctionTy>
HashTableImpl<HashEntryTy, HashFunctionTy>::~HashTableImpl()
{
  free(m_Buckets);
}

template<typename HashEntryTy,
         typename HashFunctionTy>
void HashTableImpl<HashEntryTy, HashFunctionTy>::init(unsigned int pInitSize)
{
  m_NumOfBuckets = pInitSize? compute_bucket_count(pInitSize): NumOfInitBuckets;
  
  m_NumOfItems = 0;
  m_NumOfTombstones = 0;

  m_Buckets = (BucketTy*)calloc(m_NumOfBuckets, sizeof(BucketTy));
}

template<typename HashEntryTy,
         typename HashFunctionTy>
bool HashTableImpl<HashEntryTy, HashFunctionTy>::empty() const
{
  return (0 == m_NumOfItems);
}

template<typename HashEntryTy,
         typename HashFunctionTy>
unsigned int
HashTableImpl<HashEntryTy, HashFunctionTy>::lookUpBucketFor(llvm::StringRef pKey)
{
  if (0 == m_NumOfBuckets) {
    // NumOfBuckets is changed after init(pInitSize)
    init(NumOfInitBuckets);
  }

  HashFunctionTy hasher;
  unsigned int full_hash = hasher(pKey);

  // Since m_NumOfBuckets is a power of 2, modulo can be bit operations
  unsigned int index = full_hash % m_NumOfBuckets;

  unsigned int probe = 1;
  int firstTombstone = -1;

  // quadratic probing
  while(true) {
    BucketTy &bucket = m_Buckets[index];
    // If we found an empty bucket, this key isn't in the table yet, return it.
    if (BucketTy::getEmptyBucket() == bucket.Item) {
      if (-1 != firstTombstone) {
        m_Buckets[firstTombstone].FullHashValue = full_hash;
        return firstTombstone;
      }

      bucket.FullHashValue = full_hash;
      return index;
    }
    if (BucketTy::getTombstone() == bucket.Item) {
      if (-1 == firstTombstone)
        firstTombstone = index;
    }
    else if (bucket.FullHashValue == full_hash) {
      // get string, compare, if match, return index
      if (bucket.Item->equalKey(pKey))
        return index;
    }

    index = (index+probe) % m_NumOfBuckets; //& (m_NumOfBuckets-1);
    ++probe;
  }
}

template<typename HashEntryTy,
         typename HashFunctionTy>
int
HashTableImpl<HashEntryTy, HashFunctionTy>::findKey(llvm::StringRef pKey) const
{
  if (0 == m_NumOfBuckets)
    return -1;

  HashFunctionTy hasher;
  unsigned int full_hash = hasher(pKey);
  unsigned int index = full_hash % m_NumOfBuckets;//& (m_NumOfBuckets-1);

  unsigned int probe = 1;
  // quadratic probing
  while (true) {
    BucketTy &bucket = m_Buckets[index];

    if (BucketTy::getEmptyBucket() == bucket.Item)
      return -1;

    if (BucketTy::getTombstone() == bucket.Item) {
      // Ignore tombstones.
    }
    else if (full_hash == bucket.FullHashValue) {
      if (bucket.Item->equalKey(pKey))
        return index;
    }
    index = (index + probe) % m_NumOfBuckets;//& (m_NumOfBuckets-1);
    ++probe;
  }
}

template<typename HashEntryTy,
         typename HashFunctionTy>
void HashTableImpl<HashEntryTy, HashFunctionTy>::remove(const HashEntryTy& pEntry)
{
  int index = findKey(pEntry.getKey());
  if (-1 == index)
    return 0;

  m_Buckets[index].Item = BucketTy::getTombstone();
  --m_NumOfItems;
  ++m_NumOfTombstones;
  assert(m_NumOfItems + m_NumOfTombstones <= m_NumOfBuckets);
}

template<typename HashEntryTy,
         typename HashFunctionTy>
void HashTableImpl<HashEntryTy, HashFunctionTy>::rehash()
{
  unsigned int new_size;
  if (m_NumOfItems*4 > m_NumOfBuckets*3)
    new_size = compute_bucket_count(m_NumOfBuckets);
  else if (m_NumOfBuckets-(m_NumOfItems+m_NumOfTombstones) < m_NumOfBuckets/8)
    new_size = m_NumOfBuckets;
  else
    return;

  doRehash(new_size);
}

template<typename HashEntryTy,
         typename HashFunctionTy>
void HashTableImpl<HashEntryTy, HashFunctionTy>::doRehash(unsigned int pNewSize)
{
  BucketTy* new_table = (BucketTy*)calloc(pNewSize, sizeof(BucketTy));

  // Rehash all the items into their new buckets.  Luckily :) we already have
  // the hash values available, so we don't have to rehash any strings.
  for (BucketTy *IB = m_Buckets, *E = m_Buckets+m_NumOfBuckets; IB != E; ++IB) {
    if (IB->Item && IB->Item != BucketTy::getTombstone()) {
      // Fast case, bucket available.
      unsigned full_hash = IB->FullHashValue;
      unsigned new_bucket = full_hash % pNewSize; //& (pNewSize-1);
      if (BucketTy::getEmptyBucket() == new_table[new_bucket].Item) {
        new_table[new_bucket].Item = IB->Item;
        new_table[new_bucket].FullHashValue = full_hash;
        continue;
      }

      // Otherwise probe for a spot.
      unsigned int probe = 1;
      do {
        new_bucket = (new_bucket + probe++) % pNewSize; //& (pNewSize-1);
      } while (new_table[new_bucket].Item);

      // Finally found a slot.  Fill it in.
      new_table[new_bucket].Item = IB->Item;
      new_table[new_bucket].FullHashValue = full_hash;
    }
  }

  free(m_Buckets);

  m_Buckets = new_table;
  m_NumOfBuckets = pNewSize;
  m_NumOfTombstones = 0;
}

