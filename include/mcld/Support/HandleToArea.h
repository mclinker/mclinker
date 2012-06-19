//===- HandleToArea.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_FILE_HANDLE_TO_MEMORY_AREA_H
#define MCLD_FILE_HANDLE_TO_MEMORY_AREA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/ADT/Uncopyable.h>
#include <mcld/ADT/HashBase.h>
#include <mcld/ADT/TypeTraits.h>
#include <mcld/Support/Path.h>
#include <mcld/Support/FileHandle.h>
#include <map>

namespace mcld
{

class MemoryArea;

/** \class HandleToArea
 *
 *  Special double-key associative container. Keys are Path and file handler,
 *  associative value is MemoryArea.
 *
 *  Like FileHandle, HandleToArea should neither throw exception nor call
 *  expressive diagnostic.
 */
class HandleToArea : private Uncopyable
{
public:
  struct Result {
    FileHandle* handle;
    MemoryArea* area;
  };

  struct ConstResult {
    const FileHandle* handle;
    const MemoryArea* area;
  };

public:
  HandleToArea();

  ~HandleToArea();

  bool insert(FileHandle* pHandle, MemoryArea* pArea);

  Result find(int pHandler);

  ConstResult find(int pHandler) const;

  Result find(const sys::fs::Path& pPath);

  ConstResult find(const sys::fs::Path& pPath) const;

  // -----  capacity  ----- //
  bool empty() const
  { return m_AreaMap.empty(); }

  size_t size() const
  { return m_AreaMap.size(); }

private:
  typedef HashBucket<FileHandle> Bucket;

  struct Compare {
    bool operator() (const Bucket& X, const Bucket& Y) const {
      if (X.FullHashValue < Y.FullHashValue)
        return true;

      if (X.Entry->handler() < Y.Entry->handler())
        return true;

      return (::strcmp(X.Entry->path().native().c_str(),
               Y.Entry->path().native().c_str()) < 0);
    }
  };

  // sorted binary search tree.
  typedef std::map<Bucket, MemoryArea*, Compare> HandleToAreaMap;

private:
  HandleToAreaMap m_AreaMap;
};

} // namespace of mcld

#endif

