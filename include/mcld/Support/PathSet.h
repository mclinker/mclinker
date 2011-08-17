/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#ifndef MCLD_PATH_SET_H
#define MCLD_PATH_SET_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <vector>
#include <mcld/ADT/TypeTraits.h>
#include <mcld/Support/Path.h>

namespace mcld {
namespace sys {
namespace fs {

/** \class PathSet
 *  \brief PathSet provides an efficient set of Path
 *
 *  @see sys::fs::Path
 *  @Directory
 */
class PathSet
{
private:
  class EntryTy
  {
  public:
    EntryTy()
    : length(0), path(0)
    { }

    EntryTy(const Path& pPath)
    : length(pPath.native().size()), path(&pPath)
    { }

    ~EntryTy()
    { }

  public:
    unsigned int length;
    mutable Path* path;
  };

public:
  typedef std::vector<EntryTy*> EntryList;

private:
  template<typename Traits>
  class Iterator
  {
  friend class PathSet;
  public:
    typedef Traits                          traits;
    typedef typename traits::pointer        pointer;
    typedef typename traits::reference      reference;
    typedef Iterator<Traits>  Self;

    typedef typename traits::nonconst_traits nonconst_traits;
    typedef Iterator<nonconst_traits>        iterator;
    typedef typename traits::const_traits    const_traits;
    typedef Iterator<const_traits>           const_iterator;
    typedef std::forward_iterator_tag        iterator_category;
    typedef size_t                           size_type;
    typedef ptrdiff_t                        difference_type;

  private:
    Iterator(EntryList* pParent, unsigned int pIdx)
    : m_pParent(pParent), m_Idx(pIdx)
    { }

  public:
    Iterator()
    : m_Idx(0), m_pParent(0)
    { }

    ~Iterator()
    { }

    Iterator(const Iterator& pIter)
    : m_Idx(pIter.m_Idx), m_pParent(pIter.m_pParent)
    { }

    Iterator& operator=(const Iterator& pIter) {
      m_Idx = pIter.m_Idx;
      m_pParent = pIter.m_pParent;
      return *this;
    }

    Iterator& operator++() {
      ++m_Idx;
      return *this;
    }

    Iterator operator++(int) {
      Iterator result(*this);
      ++m_Idx;
      return result;
    }

    pointer operator*() {
      return m_pParent->at(m_Idx)->path;
    }

    reference operator->() {
      return *m_pParent->at(m_Idx)->path;
    }

  private:
    unsigned int m_Idx;
    EntryList* m_pParent;
  };

public:
  typedef Iterator<NonConstTraits<Path> > iterator;
  typedef Iterator<ConstTraits<Path> >    const_iterator;

public:
  PathSet();
  PathSet(const PathSet& pCopy);
  ~PathSet();
  PathSet& operator=(const PathSet& pCopy);

  // -----  modifiers  ----- //
  void push(const Path& pPath);
  void erase();
  bool erase(const Path& pPath);

  // -----  observers  ----- //
  iterator find(const Path& pPath);
  const_iterator find(const Path& pPath) const;
  bool empty() const;
  size_t size() const;

  // -----  iterators  ----- //
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

private:
  EntryList m_Set;
};

} // namespace of fs
} // namespace of sys
} // namespace of mcld

#endif

