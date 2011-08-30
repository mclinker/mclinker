/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 *   Nowar Gu <nowar100@gmail.com>                                           *
 ****************************************************************************/
#ifndef LD_STRINGTABLE_H
#define LD_STRINGTABLE_H
#include <cassert>
#include <cstring>
#include <string>
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

/** \class StringTable
 *  \brief
 *
 *  \see
 *  \author TDYa127 <a127a127@gmail.com>
 */
class StringTable
{
public:
  StringTable() {}
  ~StringTable() {}

private:
  StringTable(const StringTable&);  // DO NOT IMPLEMENT
  StringTable& operator=(const StringTable&); // DO NOT IMPLEMENT

public:
  inline const char *getCString(size_t pIndex) const;

  size_t addCString(const char* pStr)
  { return addCString(pStr, strlen(pStr)+1);  } // We need to keep '\0' too.

  size_t size() const
  { return m_Str.size();  }

private:
  inline size_t addCString(const char *pStr, size_t pStrSize);

private:
  std::string m_Str;
};


const char* StringTable::getCString(size_t pIndex) const
{
  assert(pIndex < m_Str.size() && "Index should smaller than size.");
  return m_Str.c_str() + pIndex;
}

size_t StringTable::addCString(const char* pStr, size_t pSize)
{
  size_t ret = m_Str.size();
  m_Str += std::string(pStr, pSize);
  return ret;
}

} // namespace of mcld

#endif
