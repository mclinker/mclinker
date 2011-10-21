//===- StringTable.h ------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef LDSTRINGTABLE_H
#define LDSTRINGTABLE_H
#include <vector>
#include "mcld/LD/StrSymPool.h"
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class StringTable
 *  \brief String table, which can new different instances.
 *
 *  \see
 */
class StringTable
{
private:
  typedef std::vector<const char*>    TrackType;

public:
  typedef TrackType::const_iterator   const_iterator;
  typedef TrackType::iterator         iterator;

public:
  StringTable(StrSymPool& pImpl)
  : f_Storage(pImpl) {}

  virtual ~StringTable() {}

  const char* insert(const char* pStr)
  {
    const char* p = f_Storage.insertString(pStr);
    insertExisted(p);
    return p;
  }

  void insertExisted(const char* pStr)
  {
    f_Track.push_back(pStr);
  }

  size_t size() const { return f_Track.size(); }

  // --- Iterators --- //
  iterator begin() { return f_Track.begin(); }
  const_iterator begin() const { return f_Track.begin(); }

  iterator end() { return f_Track.end(); }
  const_iterator end() const { return f_Track.end(); }

private:
  StrSymPool& f_Storage;
  TrackType f_Track;
};

} // namespace of mcld

#endif
