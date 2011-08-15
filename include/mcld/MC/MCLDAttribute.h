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
#include <vector>

namespace mcld
{
class AttributeFactory;

/** \class AttributeBase
 *  \brief The base class of attributes. Providing the raw operations of an
 *  attributes
 */
class AttributeBase
{
public:
  AttributeBase()
  : m_WholeArchive(false),
    m_AsNeeded(false),
    m_Static(false)
  { }

  AttributeBase(const AttributeBase& pBase)
  : m_WholeArchive(pBase.m_WholeArchive),
    m_AsNeeded(pBase.m_AsNeeded),
    m_Static(pBase.m_Static)
  { }

  ~AttributeBase()
  { }

  // ----- observers  ----- //
  bool isWholeArchive() const
  { return m_WholeArchive; }

  bool isAsNeeded() const
  { return m_AsNeeded; }

  bool isStatic() const
  { return m_Static; }

  bool isDynamic() const
  { return !m_Static; }

  // -----  modifiers  ----- //
  void setWholeArchive()
  { m_WholeArchive = true; }

  void unsetWholeArchive()
  { m_WholeArchive = false; }

  void setAsNeeded()
  { m_AsNeeded = true; }

  void unsetAsNeeded()
  { m_AsNeeded = false; }

  void setStatic()
  { m_Static = true; }

  void setDynamic()
  { m_Static = false; }

private:
  bool m_WholeArchive;
  bool m_AsNeeded;
  bool m_Static;
};

/** \class MCLDAttribute
 *  \brief MCLDAttributes is the attribute of input options.
 */
class MCLDAttribute
{
private:
  friend class AttributeFactory;

  explicit MCLDAttribute(AttributeFactory& pParent, AttributeBase& pBase);
  ~MCLDAttribute();

public:
  // ----- observers  ----- //
  bool isWholeArchive() const;
  bool isAsNeeded() const;
  bool isStatic() const;
  bool isDynamic() const;

  // -----  modifiers  ----- //
  void setWholeArchive();
  void unsetWholeArchive();
  void setAsNeeded();
  void unsetAsNeeded();
  void setStatic();
  void setDynamic();

private:
  MCLDAttribute* clone() const;

  void change(AttributeBase* pBase)
  { m_pBase = pBase; }

private:
  AttributeFactory &m_Parent;
  AttributeBase *m_pBase;
};

// -----  comparisons  ----- //
inline bool operator== (const AttributeBase& pLHS, const AttributeBase& pRHS)
{
  return ((pLHS.isWholeArchive() == pRHS.isWholeArchive()) &&
    (pLHS.isAsNeeded() == pRHS.isAsNeeded()) && 
    (pLHS.isStatic() == pRHS.isStatic()));
}

inline bool operator!= (const AttributeBase& pLHS, const AttributeBase& pRHS)
{
  return !(pLHS == pRHS);
}

} // namespace of mcld

#endif

