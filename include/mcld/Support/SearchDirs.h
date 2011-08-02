/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef SEARCHDIRS_H
#define SEARCHDIRS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/StringRef.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/Path.h>

namespace mcld
{
class MCLDFile;
class MCLDDirectory;
/** \class SearchDirs
 *  \brief SearchDirs contains the list of paths that MCLinker will search for
 *  archive libraries and control scripts.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class SearchDirs
{
public:
  class iterator
  {
  public:
    MCLDDirectory& operator*();
  };

  class const_iterator
  {
  public:
    const MCLDDirectory& operator*();
  };

public:
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  sys::fs::Path find(const std::string& pNamespec) const;

  void add(const MCLDDirectory& pDirectory);
};

inline bool operator==(const SearchDirs::iterator& X, 
                       const SearchDirs::iterator& Y)
{
}

inline bool operator!=(const SearchDirs::iterator& X, 
                       const SearchDirs::iterator& Y)
{ return !(X==Y); }

} // namespace of mcld

#endif

