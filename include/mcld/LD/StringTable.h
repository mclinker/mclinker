/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   TDYa127 <a127a127@gmail.com>                                            *
 ****************************************************************************/
#ifndef STRINGTABLE_H
#define STRINGTABLE_H
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
  /* draft and test. */
public:
  inline const char *getCString(size_t pIndex) const;

  inline size_t addCString(const char *pPtr);
private:
  std::string m_Str;
};

inline const char *StringTable::getCString(size_t pIndex) const {
  return m_Str.c_str() + pIndex;
}

inline size_t StringTable::addCString(const char *pPtr) {
  size_t ret = m_Str.size();
  m_Str += pPtr;
  return ret;
}

} // namespace of mcld

#endif

