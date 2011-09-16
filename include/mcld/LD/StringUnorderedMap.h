/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef MCLD_SEARCH_TABLE_H
#define MCLD_SEARCH_TABLE_H
#include <llvm/ADT/StringRef.h>
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
#include <stdio.h>

namespace mcld
{

struct StringUnorderedMapDefaultHash
{
  size_t operator()(llvm::StringRef pStr) {
    size_t hashVal = 31;
    llvm::StringRef::iterator i = pStr.begin();
    while (i != pStr.end())
      hashVal = hashVal * 131 + (*i++);
    return hashVal;
  }
};

template<typename ValueType,
         typename StringType>
struct StringUnorderedMapInit
{
  template <typename InitType>
  void operator()(StringType &pKey, ValueType &pValue,
                  llvm::StringRef pStr, InitType pInitVal) {
    ::new ((void*)&pKey) StringType(pStr);
    ::new ((void*)&pValue) ValueType(pInitVal);
  }
};

uint32_t findNextPrime(uint32_t x);

/** \class StringUnorderedMap
 *  \brief The most simple hash of linked list version.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
template<typename ValueType = size_t,
         typename StringType = llvm::StringRef,
         typename HashFunction = StringUnorderedMapDefaultHash,
         template<class>class Allocator = std::allocator>
class StringUnorderedMap
{
  /* XXX:draft and test. */
public:
  StringUnorderedMap(size_t pMaxSize = 17);

  void reserve(size_t pMaxSize);

  template<typename InitType>
  ValueType &getOrCreate(llvm::StringRef pStr, InitType pInitVal);

  ValueType &getOrCreate(llvm::StringRef pStr) {
    return getOrCreate(pStr, ValueType());
  }

  bool empty() {return m_Size == 0;}

  void clear();

  ~StringUnorderedMap();

private:
  struct HashEntry {
    size_t hashVal;
    StringType str;
    ValueType value;
    HashEntry *next;
  };

  Allocator<HashEntry> allocator;
  HashFunction hash;
  StringUnorderedMapInit<ValueType, StringType> init;

  size_t m_HashMax;
  size_t m_Size;
  HashEntry **m_HashTable;

  void rehash(size_t pMaxSize);
};


// =================================implementation============================


template<typename ValueType,
         typename StringType,
         typename HashFunction,
         template<class>class Allocator>
StringUnorderedMap<ValueType, StringType, HashFunction, Allocator>::
StringUnorderedMap(size_t pMaxSize)
  : allocator(), hash(), init(), m_HashMax(0), m_Size(0), m_HashTable(0)
{
  this->reserve(pMaxSize);
}

template<typename ValueType,
         typename StringType,
         typename HashFunction,
         template<class>class Allocator>
void
StringUnorderedMap<ValueType, StringType, HashFunction, Allocator>::
reserve(size_t pMaxSize)
{
  if (pMaxSize < this->m_HashMax) return;
  size_t nextSize = findNextPrime(static_cast<uint32_t>(pMaxSize));
  // FIXME: Error handling.
  assert(nextSize > this->m_HashMax && "findNextPrime error.");
  if (this->m_HashMax != nextSize)
    this->rehash(nextSize);
}

template<typename ValueType,
         typename StringType,
         typename HashFunction,
         template<class>class Allocator>
void
StringUnorderedMap<ValueType, StringType, HashFunction, Allocator>::
rehash(size_t pMaxSize)
{
  HashEntry **tmpTable = new HashEntry*[pMaxSize];
  std::memset(tmpTable, 0, pMaxSize * sizeof(HashEntry*));
  if (this->m_HashTable) {
    for (size_t i = 0; i < this->m_HashMax; ++i)
      for (HashEntry *j = this->m_HashTable[i]; j != 0; ) {
        HashEntry *nextJ = j->next;
        j->next = tmpTable[j->hashVal % pMaxSize];
        tmpTable[j->hashVal % pMaxSize] = j;
        j = nextJ;
      }
    delete[] m_HashTable;
  }
  this->m_HashMax = pMaxSize;
  this->m_HashTable = tmpTable;
}

template<typename ValueType,
         typename StringType,
         typename HashFunction,
         template<class>class Allocator>
template<typename InitType>
ValueType &
StringUnorderedMap<ValueType, StringType, HashFunction, Allocator>::
getOrCreate(llvm::StringRef pStr, InitType pInitVal)
{
  size_t hashVal = hash(pStr);
  HashEntry *&head =  this->m_HashTable[hashVal % this->m_HashMax];

  HashEntry *ans = 0;
  for(HashEntry *ptr = head; ptr != 0; ptr = ptr->next)
    if(hashVal == ptr->hashVal && pStr.equals(ptr->str)) {
      ans = ptr;
      break;
    }
  if (ans == 0) {
    ans = allocator.allocate(1);
    ans->hashVal = hashVal;
    init(ans->str, ans->value, pStr, pInitVal);
    ans->next = head;
    head = ans;
    ++m_Size;
    if(this->m_Size * 4LL >= this->m_HashMax * 3LL) // load factor = 0.75
      this->reserve(this->m_HashMax+1);
  }

  return ans->value;
}

template<typename ValueType,
         typename StringType,
         typename HashFunction,
         template<class>class Allocator>
void
StringUnorderedMap<ValueType, StringType, HashFunction, Allocator>::
clear()
{
  if (this->m_HashTable) {
    for (size_t i = 0; i < this->m_HashMax; ++i)
      for (HashEntry *j = this->m_HashTable[i]; j != 0; ) {
        HashEntry *nextJ = j->next;
        allocator.destroy(j);
        allocator.deallocate(j, 1);
        j = nextJ;
      }
    delete[] m_HashTable;
  }
}


template<typename ValueType,
         typename StringType,
         typename HashFunction,
         template<class>class Allocator>
StringUnorderedMap<ValueType, StringType, HashFunction, Allocator>::
~StringUnorderedMap()
{
  this->clear();
}


} // namespace of mcld

#endif

