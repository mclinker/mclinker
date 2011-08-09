/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_ATTRIBUTE_H
#define MCLD_ATTRIBUTE_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/ADT/GCFactory.h>
#include <mcld/ADT/Uncopyable.h>
#include <vector>

namespace mcld
{

/** \class MCLDAttribute
 *  \brief MCLDAttributes is the attribute of input options.
 */
class MCLDAttribute
{
public:
  MCLDAttribute();
  ~MCLDAttribute();

  // -----  whole-archive  ----- //
  inline void setWholeArchive()
  { m_bWholeArchive = true; }

  inline void unsetWholeArchive()
  { m_bWholeArchive = false; }

  inline bool isWholeArchive() const
  { return m_bWholeArchive; }

  // -----  as-needed  ----- //
  inline bool isAsNeeded() const
  { return m_bAsNeeded; }

  inline void setAsNeeded()
  { m_bAsNeeded = true; }

  inline void unsetAsNeeded()
  { m_bAsNeeded = false; }

  bool operator== (const MCLDAttribute& pRHS) const {
    return ((m_bWholeArchive == pRHS.m_bWholeArchive) &&
            (m_bAsNeeded == pRHS.m_bAsNeeded));
  }

  bool operator!= (const MCLDAttribute& pRHS) const {
    return !(this->operator==(pRHS));
  }

private:
  bool m_bWholeArchive;
  bool m_bAsNeeded;
};

/** \class AttributeFactory
 *  \brief AttributeFactory contructs the MCLDAttributes.
 *
 *  Since the number of MCLDAttributes is usually small, a simple vector is enough.
 */
class AttributeFactory : private Uncopyable
{
private:
  typedef std::vector<MCLDAttribute*> AttrSet;

public:
  typedef AttrSet::iterator iterator;
  typedef AttrSet::const_iterator const_iterator;

public:
  explicit AttributeFactory(size_t pNum);
  ~AttributeFactory();

  MCLDAttribute* produce();

  // record - if the given attribute has been recorded, return false and 
  // change the pointer to the recorded one, and release the memory.
  bool record(MCLDAttribute *&pAttr);

  // find - return the recorded attribute whose content is identical to the
  // input. Since the number of recorded element is small, use sequential
  // search.
  MCLDAttribute* find(const MCLDAttribute& pAttr) const;

  // -----  observers  ----- //
  size_t size() const
  { return m_AttrSet.size(); }

  bool empty() const
  { return m_AttrSet.empty(); }

  // -----  iterators  ----- //
  iterator begin()
  { return m_AttrSet.begin(); }

  iterator end()
  { return m_AttrSet.end(); }

  const_iterator begin() const
  { return m_AttrSet.begin(); }

  const_iterator end() const
  { return m_AttrSet.end(); }

private:
  AttrSet m_AttrSet;
};

} // namespace of mcld

#endif

