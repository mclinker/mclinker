//===- HashTable.tcc ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

//===--------------------------------------------------------------------===//
// template implementation of HashTable
template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::HashTable(size_type pSize)
  : HashTableImpl<HashEntryTy, HashFunctionTy>(pSize), m_Alloc()
{
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::~HashTable()
{
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::begin()
{
  return iterator(this, 0);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::end()
{
  return iterator(0, 0);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::const_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::begin() const
{
  return const_iterator(this, 0);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::const_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::end() const
{
  return const_iterator(0, 0);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::bucket_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::begin(
    const typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::key_type& pKey)
{
  return bucket_iterator(this, pKey);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::bucket_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::end(
    const typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::key_type& pKey)
{
  return bucket_iterator();
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::const_bucket_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::begin(
  const typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::key_type& pKey) const
{
  return const_bucket_iterator(this, pKey);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::const_bucket_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::end(
  const typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::key_type& pKey) const
{
  return const_bucket_iterator();
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
void HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::clear()
{
  if (BaseTy::empty())
    return;
  iterator entry, eEnd = end();
  for (entry = begin(); entry != eEnd; ++entry) {
    entry.getEntry()->~HashEntryTy();
    m_Alloc.deallocate(entry.getEntry());
    entry.getEntry() = BucketTy::getEmptyBucket();
  }
  BaseTy::m_NumOfItems = 0;
  BaseTy::m_NumOfTombstones = 0;
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::value_type*
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::emplace(llvm::StringRef pString, bool& pExist)
{
  value_type* result = 0;
  unsigned int index = BaseTy::lookUpBucketFor(pString);
  BucketTy &bucket = BaseTy::m_Buckets[index];
  if (BucketTy::getEmptyBucket() != bucket.Item &&
      BucketTy::getTombstone() != bucket.Item) {
    // Already exist in the table
    pExist = true;
    return bucket.Item;
  }
  if (BucketTy::getTombstone() == bucket.Item) {
    --BaseTy::m_NumOfTombstones;
  }

  unsigned int str_len = pString.size();
  unsigned int alloc_size = sizeof(HashEntryTy)+str_len;
  result = static_cast<HashEntryTy*>(m_Alloc.allocate(alloc_size));
  new (result) HashEntryTy();
  result->setKey(pString.data(), str_len);

  bucket.Item = result;
  ++BaseTy::m_NumOfItems;

  rehash();
  pExist =false;
  return result;
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::value_type*
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::emplace(const char* pString, bool &pExist)
{
  value_type* result = 0;
  llvm::StringRef llvm_string(pString);
  unsigned int index = BaseTy::lookUpBucketFor(llvm_string);
  BucketTy &bucket = BaseTy::m_Buckets[index];
  if (BucketTy::getEmptyBucket() != bucket.Item &&
      BucketTy::getTombstone() != bucket.Item) {
    // Already exist in the table
    pExist = true;
    return bucket.Item;
  }
  if (BucketTy::getTombstone() == bucket.Item) {
    --BaseTy::m_NumOfTombstones;
  }

  unsigned int str_len = llvm_string.size();
  unsigned int alloc_size = sizeof(HashEntryTy)+str_len;
  result = static_cast<HashEntryTy*>(m_Alloc.allocate(alloc_size));
  new (result) HashEntryTy();
  result->setKey(llvm_string.data(), str_len);

  bucket.Item = result;
  ++BaseTy::m_NumOfItems;

  rehash();
  pExist = false;
  return result;
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::value_type*
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::emplace(const std::string& pString, bool &pExist)
{
  value_type* result = 0;
  llvm::StringRef llvm_string(pString);
  unsigned int index = BaseTy::lookUpBucketFor(llvm_string);
  BucketTy &bucket = BaseTy::m_Buckets[index];
  if (BucketTy::getEmptyBucket() != bucket.Item &&
      BucketTy::getTombstone() != bucket.Item) {
    // Already exist in the table
    pExist = true;
    return bucket.Item;
  }
  if (BucketTy::getTombstone() == bucket.Item) {
    --BaseTy::m_NumOfTombstones;
  }

  unsigned int str_len = llvm_string.size();
  unsigned int alloc_size = sizeof(HashEntryTy)+str_len;
  result = static_cast<HashEntryTy*>(m_Alloc.allocate(alloc_size));
  new (result) HashEntryTy();
  result->setKey(llvm_string.data(), str_len);

  bucket.Item = result;
  ++BaseTy::m_NumOfItems;

  rehash();
  pExist = false;
  return result;
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::size_type
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::erase(
        const typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::key_type& pKey)
{
  iterator bucket, bEnd = end(pKey);
  size_type counter = 0;
  for (bucket = begin(pKey); bucket != bEnd; ++bucket, ++counter) {
    bucket.getEntry()->~HashEntryTy();
    m_Alloc.deallocate(bucket.getEntry());
    bucket.getEntry() = BucketTy::getEmptyBucket();
  }
  rehash();
  return counter;
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::find(llvm::StringRef pKey)
{
  int index;
  if (-1 == (index = BaseTy::findKey(pKey)))
    return end();
  return iterator(this, index);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::const_iterator
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::find(llvm::StringRef pKey) const
{
  int index;
  if (-1 == (index = BaseTy::findKey(pKey)))
    return end();
  return const_iterator(this, index);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::size_type
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::count(
  const typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::key_type& pKey) const
{
  const_iterator bucket, bEnd = end(pKey);
  size_type count = 0;
  for (bucket = begin(pKey); bucket != bEnd; ++bucket)
    ++count;
  return count;
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
float HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::load_factor() const
{
  return (BaseTy::m_NumOfItems/BaseTy::m_NumOfBuckets);
}

template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy>
void
HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::rehash(
       typename HashTable<HashEntryTy, HashFunctionTy, AllocatorTy>::size_type pCount)
{
  BaseTy::doRehash(pCount);
}

