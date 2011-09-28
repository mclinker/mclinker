/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LDSTRINGTABLEIF_H
#define LDSTRINGTABLEIF_H
#include <vector>
#include <mcld/LD/StrSymPool.h>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class StringTableIF
 *  \brief String table interface.
 *
 *  \see
 *  \author Nowar Gu <nowar100@gmail.com>
 */
class StringTableIF
{
private:
  typedef std::vector<const char*>      TrackType;
protected:
  StringTableIF(StrSymPool& pImpl)
  : f_Storage(pImpl) {}

public:
  typedef TrackType::const_iterator   const_iterator;
  typedef TrackType::iterator         iterator;

public:
  virtual ~StringTableIF() {}

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

protected:
  StrSymPool& f_Storage;
  TrackType f_Track;
};

} // namespace of mcld

#endif
