/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_ATTRIBUTE_FACTORY_H
#define MCLD_ATTRIBUTE_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/ADT/Uncopyable.h>
#include <mcld/MC/MCLDAttribute.h>

namespace mcld
{

/** \class AttributeFactory
 *  \brief AttributeFactory contructs the MCLDAttributes.
 *
 *  Since the number of MCLDAttributes is usually small, sequential search
 *  on a small vector is enough.
 */
class AttributeFactory : private Uncopyable
{
private:
  typedef std::vector<AttributeBase*> AttrSet;

public:
  typedef AttrSet::iterator iterator;
  typedef AttrSet::const_iterator const_iterator;

public:
  AttributeFactory();
  explicit AttributeFactory(size_t pNum);
  ~AttributeFactory();

  // reserve - reserve the memory space for attributes
  // @param pNum the number of reserved attributes
  void reserve(size_t pNum);

  // predefined - return the predefined attribute
  AttributeBase& predefined();
  const AttributeBase& predefined() const;
  
  // constraint - return the constraint of attributes
  AttributeBase& constraint()
  { return m_Constraint; }

  const AttributeBase& constraint() const
  { return m_Constraint; }

  // produce - produce a attribute, but do not record it yet.
  // the produced attribute is identical to the pre-defined attribute.
  MCLDAttribute* produce();

  // last - the last touched attribute.
  MCLDAttribute& last();
  const MCLDAttribute& last() const;

  // exists- return the recorded attribute whose content is identical to the
  // input attribute.
  AttributeBase *exists(const AttributeBase& pAttr) const;

  // record - record the attribute no mater if it has been recorded.
  void record(AttributeBase& pAttr);

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
  AttributeBase m_Constraint;
  MCLDAttribute *m_pLast;
};

} // namespace of mcld

#endif

