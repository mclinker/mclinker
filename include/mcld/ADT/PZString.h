/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef MCLD_PASCAL_ZERO_END_STRING_H
#define MCLD_PASCAL_ZERO_END_STRING_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <string>
#include <PZStringFactory.h>

namespace mcld
{

/** \class PZString
 *  \brief PZString is a Pascal string with a zero terminal.
 *
 *  Because PZString is a kind of class whose length is various in run-time,
 *  PZString must be put into heap. The class having a PZString as a member
 *  variable must be put into heap, too.
 */
class PZString
{
friend class PZStringFactory;
private:
  PZString();
  ~PZString();

public:
  typedef const char *iterator;
  typedef const char *const_iterator;
  static const size_t npos = ~size_t(0);
  typedef size_t size_type;
  typedef PZStringFactory Alloc;

public:
  // -----  iterators  ----- //
  iterator begin()
  { return m_Data; }

  iterator end()
  { return m_Data + m_Length; }

  const_iterator begin() const
  { return m_Data; }

  const_iterator end() const
  { return m_Data + m_Length; }

  iterator rbegin();
  iterator rend();
  const_iterator rbegin() const;
  const_iterator rend() const;

  // -----  capacity  ----- //
  size_type size() const
  { return m_Length; }

  size_type length() const
  { return m_Length; }

  bool empty() const
  { return m_Length == 0; }

  // -----  element access  ----- //
  const char& operator[](size_type pPos) const
  { return m_Data[pPos]; }

  char& operator[](size_type pPos)
  { return m_Data[pPos]; }

  const char& at(size_type pPos) const;
  char& at(size_type pPos);

  // -----  modifiers  ----- //
  PZString& assign(const char* pStr, Alloc& pFactory);
  PZString& assign(const char* pStr, size_type pLength, Alloc& pFactory);
  PZString& assign(const char* pStart, const char* pLast, Alloc& pFactory);
  PZString& assign(const std::string& pStr, Alloc& pFactory);
  PZString& assign(const llvm::StringRef& pStr, Alloc& pFactory);
  PZString& assign(const PZString& pCopy, Alloc& pFactory);

  // -----  string operations  ----- //
  const char* data() const
  { return m_Data; }

  const char* c_str() const
  { return m_Data; }

private:
  size_type m_Length;
  char m_Data[0];
};

} // namespace of mcld

#endif

