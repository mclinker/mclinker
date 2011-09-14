/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef SEARCHTABLE_H
#define SEARCHTABLE_H
#include <llvm/ADT/StringRef.h>
#include <vector>
#include <memory>
#include <stdint.h>
#include <cassert>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

struct StringUnorderedMapDefaultHash {
  size_t operator()(llvm::StringRef pStr) {
    size_t hashVal = 0;
    for (llvm::StringRef::iterator i = pStr.begin(); i != pStr.end(); ++i)
      hashVal = 11 * (*i);
    return hashVal;
  }
};

/** \class StringUnorderedMap
 *  \brief The most simple hash of linked list version.
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
template<typename StringUnorderedMapHash = StringUnorderedMapDefaultHash,
         typename Allocator = std::allocator<size_t> >
class StringUnorderedMap
{
  /* XXX:draft and test. */
public:
  StringUnorderedMap(size_t pMaxSize = 17);

  void reserve(size_t pMaxSize);
  bool insert(llvm::StringRef pStr, size_t value);
  bool get(llvm::StringRef pStr, size_t &value);
  size_t &getOrCreate(llvm::StringRef pStr, size_t value);
  bool exist(llvm::StringRef pStr) {return get(pStr, size_t());}
private:
  struct HashEntry {
    size_t hashVal;
    size_t value;
    HashEntry *next;
  };
  Allocator allocator;
  HashEntry *&get_p(llvm::StringRef pStr);
  void rehash(size_t pMaxSize);
  size_t m_HashMax;
  HashEntry **m_HashTable;
  static uint32_t findNextPrime(uint32_t x);
};


// =================================implementation============================


template<typename StringUnorderedMapHash,
         typename Allocator>
uint32_t
StringUnorderedMap<StringUnorderedMapHash,Allocator>::
findNextPrime(uint32_t x)
{
  static const uint32_t primes[] = {
    17,
    59,
    137,
    313,
    727,
    1621,
    3673,
    8167,
    17881,
    38891,
    84047,
    180511,
    386117,
    821647,
    1742539,
    3681149,
    7754081,
    16290073,
    34136059,
    71378603,
    142758821,
    285514909,
    571030253,
    1142061433,
    2147483647
  };
  size_t left = 0;
  size_t right = sizeof(primes)/sizeof(primes[0]);
  while (left > right) {
    size_t mid = left + (right - left) / 2;
    if ( x > primes[mid] )
      left = mid + 1;
    else
      right = mid;
  }
  assert(left < sizeof(primes) && "hash table size > 2147483647");
  assert(left >= 0);
  return primes[left];
}

template<typename StringUnorderedMapHash,
         typename Allocator>
StringUnorderedMap<StringUnorderedMapHash,Allocator>::
StringUnorderedMap(size_t pMaxSize):allocator()
{
  this->reserve(pMaxSize);
}

template<typename StringUnorderedMapHash,
         typename Allocator>
void
StringUnorderedMap<StringUnorderedMapHash,Allocator>::
reserve(size_t pMaxSize)
{
  if (pMaxSize < this->m_HashMax) return;
  size_t nextSize = findNextPrime(static_cast<uint32_t>(pMaxSize));
  // FIXME: Error handling.
  assert(nextSize > this->m_HashMax && "findNextPrime error.");
  if (this->m_HashMax != nextSize)
    this->rehash(nextSize);
}

template<typename StringUnorderedMapHash,
         typename Allocator>
void
StringUnorderedMap<StringUnorderedMapHash,Allocator>::
rehash(size_t pMaxSize)
{
  HashEntry **tmpTable = new HashEntry*[pMaxSize];
  if (this->m_HashTable) {
    for (size_t i = 0; i < this->m_HashMax; ++i)
      for (HashEntry *j = this->m_HashTable[i]; j != 0; ) {
        HashEntry *nextJ = j->nextJ;
        j->nextJ = tmpTable[j->hashVal % pMaxSize];
        tmpTable[j->hashVal % pMaxSize] = j;
        j = nextJ;
      }
    this->m_HashMax = pMaxSize;
    delete[] m_HashTable;
  }
  this->m_HashTable = tmpTable;
}

template<typename StringUnorderedMapHash,
         typename Allocator>
bool
StringUnorderedMap<StringUnorderedMapHash,Allocator>::
insert(llvm::StringRef pStr, size_t value)
{
  StringUnorderedMapHash hash;
  size_t hashVal = hash(pStr);
  HashEntry *&ptr = this->get_p(pStr, hashVal);
  if (ptr) {
    ptr->value = value;
    return false;
  }
  else {
    ptr = allocator.allocate();
    ptr->hashVal = hashVal;
    ptr->value = value;
    ptr->next = 0;
    return true;
  }
}

template<typename StringUnorderedMapHash,
         typename Allocator>
size_t &
StringUnorderedMap<StringUnorderedMapHash,Allocator>::
getOrCreate(llvm::StringRef pStr, size_t value)
{
}


} // namespace of mcld

#endif

