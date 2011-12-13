//===- StringUnorderedMap.h -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SEARCH_TABLE_H
#define MCLD_SEARCH_TABLE_H
#include <vector>
// For std::allocate.
#include <memory>
// For uint32_t.
#include <stdint.h>
#include <cassert>
// For memset.
#include <cstring>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
/* FIXME: Move StringUnorderedMap under ADT. */

namespace mcld
{

struct StringUnorderedMapDefaultHash
{
  size_t operator()(const char *pStr) {
    size_t hashVal = 31;
    while (*pStr)
      hashVal = hashVal * 131 + (*pStr++);
    return hashVal;
  }
};

template<typename ValueType,
         typename KeyType>
struct StringUnorderedMapEntryInit
{
  template <typename InitType>
  void operator()(KeyType &pKey, ValueType &pValue,
                  const KeyType &pStr, InitType pInitVal) {
    ::new ((void*)&pKey) KeyStorageType(pStr);
    ::new ((void*)&pValue) ValueType(pInitVal);
  }
};

uint32_t findNextPrime(uint32_t x);

/** \class StringUnorderedMap
 *  \brief The most simple hash of linked list version.
 *
 *  \see
 */
template<typename KeyType,
         typename ValueType,
         typename KeyCompareFunctor,
         typename HashFunction = StringUnorderedMapDefaultHash,
         typename Allocator = std::allocator<std::pair<KeyType, ValueType> > >
class StringUnorderedMap
{
public:
  explicit StringUnorderedMap(size_t pCapacity = 17)
  : m_Impl(pCapacity)
  {}

  ~StringUnorderedMap();

  void reserve(size_t pCapacity);

  ValueType &getOrCreate(const KeyType &pStr, const ValueType &pInitVal);

  ValueType &getOrCreate(const KeyType &pStr);

  bool empty()
  { return m_Size == 0; }

  size_t capacity() const
  { return m_Capacity; }

  void clear();

private:
  struct HashEntry {
    size_t hashVal;
    std::pair<KeyType, ValueType>
    HashEntry *next;
  };
  typedef Allocator::template rebind<HashEntry>::other HashEntryAllocator;
  void rehash(size_t pCapacity);

private:
  size_t m_Capacity;
  size_t m_Size;
  // array of pointers to hash entries
  HashEntry **m_HashTable;
  HashEntryAllocator m_HashEntryAllocator;
};


// =================================implementation============================
// StringUnorderedMap::StringUnorderedMapImpl
template<typename ValueType,
         typename KeyStorageType,
         typename HashFunction,
         typename Allocator>
StringUnorderedMap<ValueType, KeyStorageType, HashFunction, Allocator>::
StringUnorderedMapImpl::StringUnorderedMapImpl(size_t pCapacity)
  : m_Capacity(0), m_Size(0), m_HashTable(0)
{
  this->reserve(pCapacity);
}

template<typename ValueType,
         typename KeyStorageType,
         typename HashFunction,
         typename Allocator>
void
StringUnorderedMap<ValueType, KeyStorageType, HashFunction, Allocator>::
StringUnorderedMapImpl::reserve(size_t pCapacity)
{
  if (pCapacity < this->m_Capacity)
    return;
  size_t nextSize = findNextPrime(static_cast<uint32_t>(pCapacity));
  // FIXME: Error handling.
  assert(nextSize > this->m_Capacity && "findNextPrime error.");
  if (this->m_Capacity != nextSize)
    this->rehash(nextSize);
}

template<typename ValueType,
         typename KeyStorageType,
         typename HashFunction,
         typename Allocator>
void
StringUnorderedMap<ValueType, KeyStorageType, HashFunction, Allocator>::
StringUnorderedMapImpl::rehash(size_t pCapacity)
{
  HashEntry **tmpTable = new HashEntry*[pCapacity];
  std::memset(tmpTable, 0, pCapacity * sizeof(HashEntry*));
  if (this->m_HashTable) {
    for (size_t i = 0; i < this->m_Capacity; ++i)
      for (HashEntry *j = this->m_HashTable[i]; j != 0; ) {
        HashEntry *nextJ = j->next;
        j->next = tmpTable[j->hashVal % pCapacity];
        tmpTable[j->hashVal % pCapacity] = j;
        j = nextJ;
      }
    delete[] m_HashTable;
  }
  this->m_Capacity = pCapacity;
  this->m_HashTable = tmpTable;
}

template<typename ValueType,
         typename KeyStorageType,
         typename HashFunction,
         typename Allocator>
template<typename InitType>
ValueType &
StringUnorderedMap<ValueType, KeyStorageType, HashFunction, Allocator>::
StringUnorderedMapImpl::getOrCreate(const KeyType &pStr, ValueType &pInitVal)
{
  HashFunction hash;
  size_t hashVal = hash(pStr);
  HashEntry *&head =  this->m_HashTable[hashVal % this->m_Capacity];

  HashEntry *ans = 0;
  for(HashEntry *ptr = head; ptr != 0; ptr = ptr->next)
    if(hashVal == ptr->hashVal && pStr.equals(ptr->str)) {
      ans = ptr;
      break;
    }
  if (ans == 0) {
    ans = this->allocate(1);
    ans->hashVal = hashVal;
    StringUnorderedMapEntryInit<ValueType, KeyStorageType> init;
    init(ans->str, ans->value, pStr, pInitVal);
    ans->next = head;
    head = ans;
    ++m_Size;
    if(this->m_Size * 4LL >= this->m_Capacity * 3LL) // load factor = 0.75
      this->reserve(this->m_Capacity+1);
  }

  return ans->value;
}

template<typename ValueType,
         typename KeyStorageType,
         typename HashFunction,
         typename Allocator>
void
StringUnorderedMap<ValueType, KeyStorageType, HashFunction, Allocator>::
StringUnorderedMapImpl::clear()
{
  if (this->m_HashTable) {
    for (size_t i = 0; i < this->m_Capacity; ++i)
      for (HashEntry *j = this->m_HashTable[i]; j != 0; ) {
        HashEntry *nextJ = j->next;
        this->destroy(j);
        this->deallocate(j, 1);
        j = nextJ;
      }
    delete[] m_HashTable;
  }
}


template<typename ValueType,
         typename KeyStorageType,
         typename HashFunction,
         typename Allocator>
StringUnorderedMap<ValueType, KeyStorageType, HashFunction, Allocator>::
StringUnorderedMapImpl::~StringUnorderedMapImpl()
{
  this->clear();
}


} // namespace of mcld

#endif

