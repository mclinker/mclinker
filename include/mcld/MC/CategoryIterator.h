//===- CategoryIterator.h -------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_SYMBOL_CATEGORY_ITERATOR_H
#define MCLD_SYMBOL_CATEGORY_ITERATOR_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/ADT/TypeTraits.h>

using namespace std;

namespace mcld
{

class LDSymbol;

template<typename DATA_TRAITS,
         typename POOL_TRAITS,
         typename FORCELOCAL_TRAITS,
         typename OUTPUTSYMBOL_TRAITS>
class CategoryIterator
{
public:
  typedef typename DATA_TRAITS::value_type value_type;
  typedef typename DATA_TRAITS::pointer pointer;
  typedef typename DATA_TRAITS::reference reference;

  typedef CategoryIterator<DATA_TRAITS,
                           POOL_TRAITS,
                           FORCELOCAL_TRAITS,
                           OUTPUTSYMBOL_TRAITS> Self;

  typedef typename POOL_TRAITS::value_type Pool;
  typedef typename POOL_TRAITS::pointer PoolPtr;
  typedef typename FORCELOCAL_TRAITS::value_type ForceLocals;
  typedef typename OUTPUTSYMBOL_TRAITS::value_type OutputSymbols;

private:
  friend class POOL_TRAITS::value_type;

  CategoryIterator(PoolPtr pPool, size_t pIdx)
  : m_pPool(pPool) {
    if (NULL == m_pPool)
      return;

    if (0 == pIdx) {
      if (false == m_pPool->emptyForceLocals()) {
        m_FLocal = m_pPool->forceLocalBegin();
        m_Symbol = m_pPool->regularEnd();
        m_State = ForceLocal;
      }
      else if (!m_pPool->emptyLocals()) {
        m_Symbol = m_pPool->localBegin();
        m_State = LocalSymbol;
      }
      else if (!m_pPool->emptyCommons()) {
        m_Symbol = m_pPool->commonBegin();
        m_State = CommonSymbol;
      }
      else if (!m_pPool->emptyRegulars()) {
        m_Symbol = m_pPool->regularBegin();
        m_State = RegularSymbol;
      }
      else {
        m_FLocal = m_pPool->forceLocalEnd();
        m_Symbol = m_pPool->regularEnd();
        m_State = OutOfState;
      }
      return;
    }

    if (-1 == pIdx) {
      m_Symbol = m_pPool->regularEnd();
      m_State = OutOfState;
      return;
    }
  }

public:
  CategoryIterator()
  : m_pPool(NULL), m_State(OutOfState)
  { }

  CategoryIterator(const CategoryIterator& pCopy)
    : m_pPool(pCopy.m_pPool),
      m_State(pCopy.m_State) {
    m_FLocal = pCopy.m_FLocal;
    m_Symbol = pCopy.m_Symbol;
  }

  CategoryIterator& operator=(const CategoryIterator& pCopy) {
    m_pPool = pCopy.m_pPool;
    m_FLocal = pCopy.m_FLocal;
    m_Symbol = pCopy.m_Symbol;
    m_State = pCopy.m_State;
    return *this;
  }

  ~CategoryIterator()
  { }

  Self& operator++() {
    if (NULL == m_pPool || OutOfState == m_State)
      return *this;

    switch(m_State) {
      case ForceLocal: {
        ++m_FLocal;
        if (m_pPool->forceLocalEnd() == m_FLocal)
          goNextState(m_State);
        break;
      }
      case LocalSymbol: {
        ++m_Symbol;
        if (m_pPool->localEnd() == m_Symbol)
          goNextState(m_State);
        break;
      }
      case CommonSymbol: {
        ++m_Symbol;
        if (m_pPool->commonEnd() == m_Symbol)
          goNextState(m_State);
        break;
      }
      case RegularSymbol: {
        ++m_Symbol;
        if (m_pPool->regularEnd() == m_Symbol)
          goNextState(m_State);
        break;
      }
      case OutOfState:
      default: {
        break;
      }
    }
    return *this;
  }

  Self operator++(int) {
    Self tmp(*this);
    if (NULL == m_pPool || OutOfState == m_State)
      return tmp;

    switch(m_State) {
      case ForceLocal: {
        ++m_FLocal;
        if (m_pPool->forceLocalEnd() == m_FLocal)
          goNextState(m_State);
        break;
      }
      case LocalSymbol: {
        ++m_Symbol;
        if (m_pPool->localEnd() == m_Symbol)
          goNextState(m_State);
        break;
      }
      case CommonSymbol: {
        ++m_Symbol;
        if (m_pPool->commonEnd() == m_Symbol)
          goNextState(m_State);
        break;
      }
      case RegularSymbol: {
        ++m_Symbol;
        if (m_pPool->regularEnd() == m_Symbol)
          goNextState(m_State);
        break;
      }
      case OutOfState:
      default: {
        break;
      }
    }
    return tmp;
  }

  bool operator==(const CategoryIterator& pOther) const {
    if (m_pPool != pOther.m_pPool)
      return false;
    if (m_State != pOther.m_State)
      return false;
    return true;
  }

  bool operator!=(const CategoryIterator& pOther) const {
    return !this->operator==(pOther);
  }

  pointer operator*() const {
    if (m_pPool == NULL)
      return NULL;
    if (OutOfState == m_State)
      return NULL;

    if (ForceLocal == m_State)
      return *m_FLocal;

    return *m_Symbol;
  }

private:
  enum State {
    ForceLocal    = 0,
    LocalSymbol   = 1,
    CommonSymbol  = 2,
    RegularSymbol = 3,
    OutOfState
  };

  void goNextState(State pCurrentState) {
    switch(pCurrentState) {
      case ForceLocal: {
        if (!m_pPool->emptyLocals()) {
          m_Symbol = m_pPool->localBegin();
          m_State = LocalSymbol;
          break;
        }
      }
      case LocalSymbol: {
        if (!m_pPool->emptyCommons()) {
          m_Symbol = m_pPool->commonBegin();
          m_State = CommonSymbol;
          break;
        }
      }
      case CommonSymbol: {
        if (!m_pPool->emptyRegulars()) {
          m_Symbol = m_pPool->regularBegin();
          m_State = RegularSymbol;
          break;
        }
      }
      case RegularSymbol:
      case OutOfState:
      default: {
        m_FLocal = m_pPool->forceLocalEnd();
        m_Symbol = m_pPool->regularEnd();
        m_State = OutOfState;
      }
    } // end of switch
  }
private:
  PoolPtr m_pPool;
  typename FORCELOCAL_TRAITS::iterator m_FLocal;
  typename OUTPUTSYMBOL_TRAITS::iterator m_Symbol;
  State m_State;
};

} // namespace of mcld

#endif

