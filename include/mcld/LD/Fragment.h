//===- Fragment.h ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_FRAGMENT_H
#define MCLD_FRAGMENT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/ilist_node.h>
#include <llvm/Support/DataTypes.h>

#include <cstddef>

namespace mcld
{

class SectionData;

/** \class Fragment
 *  \brief Fragment is the minimun linking unit of MCLinker.
 */
class Fragment : public llvm::ilist_node<Fragment>
{
public:
  enum Type {
    Alignment,
    Fillment,
    Region,
    Relocation,
    Target
  };

public:
  Fragment();

  Fragment(Type pKind, SectionData *pParent = NULL);

  virtual ~Fragment();

  Type getKind() const { return m_Kind; }

  SectionData *getParent() const { return m_pParent; }

  void setParent(SectionData *pValue) { m_pParent = pValue; }

  uint64_t getOffset() const { return m_Offset; }

  void setOffset(uint64_t pOffset) { m_Offset = pOffset; }

  unsigned int getLayoutOrder() const { return m_LayoutOrder; }

  void setLayoutOrder(unsigned int pValue) { m_LayoutOrder = pValue; }

  static bool classof(const Fragment *O) { return true; }

private:
  Fragment(const Fragment& );            // DO NOT IMPLEMENT
  Fragment& operator=(const Fragment& ); // DO NOT IMPLEMENT

private:
  Type m_Kind;
  SectionData* m_pParent;

  uint64_t m_Offset;
  unsigned int m_LayoutOrder;

};

} // namespace of mcld

#endif

