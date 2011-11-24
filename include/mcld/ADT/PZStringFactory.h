/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Diana Chen <diana.chen@mediatek.com>                                    *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef PASCAL_ZERO_END_STRING_FACTORY_H
#define PASCAL_ZERO_END_STRING_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/StringRef.h>
#include <string>

namespace mcld
{
class PZString;

/** \class PZStringFactory
 *  \brief PZStringFactory is the factory for PZString.
 */
class PZStringFactory
{
public:
  PZString* create(const char* pStr);
  PZString* create(const char* pStr, size_t pLength);
  PZString* create(const char* pStart, const char* pLast);
  PZString* create(const std::string& pStr);
  PZString* create(const llvm::StringRef& pStr);
  void destroy(PZString* pString);
};

} // namespace of mcld

#endif

