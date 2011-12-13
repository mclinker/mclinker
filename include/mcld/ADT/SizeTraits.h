//===- SizeTraits.h -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef SIZETRAITS_H
#define SIZETRAITS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <stdint.h>

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

