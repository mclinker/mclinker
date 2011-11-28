/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef SIZETRAITS_H
#define SIZETRAITS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

namespace mcld
{

template<int SIZE>
struct SizeTraits;

template<>
class SizeTraits<32>
{
  typedef uint32_t Address;
  typedef uint32_t Offset;
  typedef uint32_t Word;
  typedef int32_t  SWord;
};

template<>
class SizeTraits<64>
{
  typedef uint64_t Address;
  typedef uint64_t Offset;
  typedef uint64_t Word;
  typedef int64_t  SWord;
};

} // namespace of mcld

#endif

