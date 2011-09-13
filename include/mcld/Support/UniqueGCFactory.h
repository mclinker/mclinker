/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_UNIQUE_GCFACTORY_H
#define MCLD_UNIQUE_GCFACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <mcld/Support/GCFactory.h>
#include <map>
#include <utility>

namespace mcld
{

/** \class UniqueGCFactoryBase
 *  \brief UniqueGCFactories are unique associative factories, meaning that
 *  no two elements have the same key.
 */
template<typename KeyType, typename DataType, size_t ChunkSize>
class UniqueGCFactoryBase : public GCFactoryBase<LinearAllocator<DataType, ChunkSize> >
{
protected:
  typedef GCFactoryBase<LinearAllocator<DataType, ChunkSize> > Alloc;
  typedef std::map<KeyType, DataType*> KeyMap;

protected:
  UniqueGCFactoryBase()
  : GCFactoryBase<LinearAllocator<DataType, ChunkSize> >()
  { }

  UniqueGCFactoryBase(size_t pNum)
  : GCFactoryBase<LinearAllocator<DataType, ChunkSize> >(pNum)
  { }

public:
  virtual ~UniqueGCFactoryBase()
  { f_KeyMap.clear(); }

  DataType* produce(const KeyType& pKey) {
    typename KeyMap::iterator dataIter = f_KeyMap.find(pKey);
    if (dataIter != f_KeyMap.end())
      return dataIter->second;
    DataType* data = Alloc::allocate();
    construct(data);
    f_KeyMap.insert(std::make_pair(pKey, data));
    return data;
  }

  DataType* produce(const KeyType& pKey, const DataType& pValue) {
    typename KeyMap::iterator dataIter = f_KeyMap.find(pKey);
    if (dataIter != f_KeyMap.end())
      return dataIter->second;
    DataType* data = Alloc::allocate();
    construct(data, pValue);
    f_KeyMap.insert(std::make_pair(pKey, data));
    return data;
  }

protected:
  KeyMap f_KeyMap;

};

} // namespace of mcld

#endif

