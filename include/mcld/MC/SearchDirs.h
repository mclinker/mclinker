//===- SearchDirs.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef SEARCHDIRS_H
#define SEARCHDIRS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/StringRef.h>
#include "mcld/ADT/Uncopyable.h"
#include "mcld/MC/MCLDInput.h"

namespace mcld
{

class MCLDFile;
class MCLDDirectory;

namespace sys {
namespace fs {
class Path;
} // namespace of fs
} // namespace of sys

/** \class SearchDirs
 *  \brief SearchDirs contains the list of paths that MCLinker will search for
 *  archive libraries and control scripts.
 *
 *  SearchDirs is customized for linking. It handles -L on the command line
 *  and SEARCH_DIR macro in the link script.
 *
 *  @see MCLDDirectory.
 */
class SearchDirs : private Uncopyable
{
public:
  typedef std::vector<MCLDDirectory*> DirList;
  typedef DirList::iterator iterator;
  typedef DirList::const_iterator const_iterator;

public:
  SearchDirs();
  ~SearchDirs();

  // find - give a namespec, return a real path of the shared object.
  //
  sys::fs::Path* find(const std::string& pNamespec, mcld::Input::Type pPreferType);

  // -----  iterators  ----- //
  iterator begin()
  { return m_DirList.begin(); }

  iterator end()
  { return m_DirList.end(); }

  const_iterator begin() const
  { return m_DirList.begin(); }

  const_iterator end() const
  { return m_DirList.end(); }

  // -----  modifiers  ----- //
  void add(const MCLDDirectory& pDirectory);

private:
  DirList m_DirList;
};

} // namespace of mcld

#endif

