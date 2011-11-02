//===- HashTable.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_HASH_TABLE_H
#define MCLD_HASH_TABLE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include "mcld/ADT/HashEntry.h"
#include "mcld/ADT/Uncopyable.h"
#include "mcld/ADT/TypeTraits.h"
#include "mcld/ADT/StringHash.h"
#include "mcld/Support/Allocators.h"
#include "llvm/ADT/StringRef.h"
#include <string>
#include <utility>

namespace mcld
{

/** \class HashIterator
 *  \brief HashIterator provides a policy-based iterator.
 *
 *  HashTable has two kinds of iterators. One is used to traverse buckets
 *  with the same hash value; the other is used to traverse all entries of the
 *  hash table.
 *
 *  HashIterator is a template policy-based iterator, which can change its
 *  behavior by change the template argument IteratorBase. HashTable defines
 *  above two iterators by defining HashIterators with different IteratorBase.
 */
template<typename HashEntryTy,
         typename IteratorBase,
         typename Traits>
class HashIterator : public IteratorBase
{
public:
  typedef HashEntryTy                value_type;
  typedef Traits                     traits;
  typedef typename traits::pointer   pointer;
  typedef typename traits::reference reference;
  typedef size_t                     size_type;
  typedef ptrdiff_t                  difference_type;
  typedef IteratorBase               Base;

  typedef HashIterator<value_type,
                       IteratorBase,
                       Traits>             Self;

  typedef typename traits::nonconst_traits nonconst_traits;
  typedef HashIterator<value_type,
                       IteratorBase,
                       nonconst_traits>    iterator;
  typedef typename traits::const_traits    const_traits;
  typedef HashIterator<value_type,
                       IteratorBase,
                       const_traits>       const_iterator;
  typedef std::forward_iterator_tag        iterator_category;

public:
  HashIterator()
  : IteratorBase()
  { }

  HashIterator(typename IteratorBase::HashTableImplTy* pTable, llvm::StringRef pKey)
  : IteratorBase(pTable, pKey)
  { }

  HashIterator(typename IteratorBase::HashTableImplTy* pTable, unsigned int pIndex)
  : IteratorBase(pTable, pIndex)
  { }

  HashIterator(const HashIterator& pCopy)
  : IteratorBase(pCopy)
  { }

  ~HashIterator()
  { }

  // -----  operators  ----- //
  Self& operator++() {
    this->Base::advance();
    return *this;
  }

  Self operator++(int) {
    Self tmp = *this;
    this->Base::advance();
    return tmp;
  }
};


/** \class HashTable
 *  \brief HashTable is a hash table which follows boost::unordered_map.
 *
 *  mcld::HashTable is a quadratic probing hash table. It does not allocate
 *  the memory space of the entries by itself. Instead, entries are allocated
 *  outside and then emplaced into the hash table.
 */
template<typename HashEntryTy,
         typename HashFunctionTy,
         typename AllocatorTy = mcld::MallocAllocator<void> >
class HashTable : public HashTableImpl<HashEntryTy, HashFunctionTy>,
                  private Uncopyable
{
public:
  typedef size_t size_type;
  typedef HashEntryTy value_type;
  typedef HashFunctionTy hasher;
  typedef HashTableImpl<HashEntryTy, HashFunctionTy> BaseTy;
  typedef typename HashEntryTy::KeyTy key_type;
  typedef typename HashEntryTy::ValueTy mapped_type;
  typedef typename HashTableImpl<HashEntryTy, HashFunctionTy>::BucketTy BucketTy;

  typedef HashIterator<HashEntryTy,
                       BucketIteratorBase<HashEntryTy, HashFunctionTy>,
                       NonConstTraits<HashEntryTy> > bucket_iterator;

  typedef HashIterator<HashEntryTy,
                       BucketIteratorBase<HashEntryTy, HashFunctionTy>,
                       ConstTraits<HashEntryTy> > const_bucket_iterator;

  typedef HashIterator<HashEntryTy,
                       HashIteratorBase<HashEntryTy, HashFunctionTy>,
                       NonConstTraits<HashEntryTy> > iterator;

  typedef HashIterator<HashEntryTy,
                       HashIteratorBase<HashEntryTy, HashFunctionTy>,
                       ConstTraits<HashEntryTy> > const_iterator;

public:
  // -----  constructor  ----- //
  explicit HashTable(size_type pSize=3);
  ~HashTable();
  
  AllocatorTy& getAllocator()
  { return m_Alloc; }

  // -----  iterators  ----- //
  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  bucket_iterator begin(const key_type& pKey);
  bucket_iterator end(const key_type& pKey);
  const_bucket_iterator begin(const key_type& pKey) const;
  const_bucket_iterator end(const key_type& pKey) const;

  // -----  modifiers  ----- //
  void clear();

  /// emplace - insert a new element to the container. The element is
  //  constructed in-place, i.e. no copy or move operations are performed.
  //  If the element already exists, return the element, and set pExist true.
  value_type* emplace(llvm::StringRef pString, bool& pExist);
  value_type* emplace(const char* pString, bool& pExist);
  value_type* emplace(const std::string& pString, bool& pExist);

  size_type erase(const key_type& pKey);

  // -----  lookups  ----- //
  /// find - finds an element with key pKey
  //  If the element does not exist, return end()
  iterator find(llvm::StringRef pKey);

  /// find - finds an element with key pKey, constant version
  //  If the element does not exist, return end()
  const_iterator find(llvm::StringRef pKey) const;
  size_type count(const key_type& pKey) const;
  
  // -----  hash policy  ----- //
  float load_factor() const;

  using HashTableImpl<HashEntryTy, HashFunctionTy>::rehash;
  void rehash(size_type pCount);

private:
  AllocatorTy m_Alloc;

};

#include "HashTable.tcc"

} // namespace of mcld

#endif

