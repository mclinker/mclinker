//===- StringUnorderedMap.cpp ---------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <stdint.h>
#include <cassert>
#include <stdio.h>
#include "mcld/LD/StringUnorderedMap.h"

using namespace mcld;

//==========================
// StringUnorderedMap

namespace mcld
{

uint32_t findNextPrime(uint32_t x)
{
  static const uint32_t primes[] = {
    17,
    59,
    137,
    313,
    727,
    1621,
    3673,
    8167,
    17881,
    38891,
    84047,
    180511,
    386117,
    821647,
    1742539,
    3681149,
    7754081,
    16290073,
    34136059,
    71378603,
    142758821,
    285514909,
    571030253,
    1142061433,
    2147483647
  };
  size_t left = 0;
  size_t right = sizeof(primes)/sizeof(primes[0]);
  while (left < right) {
    size_t mid = left + (right - left) / 2;
    if ( x > primes[mid] )
      left = mid + 1;
    else
      right = mid;
  }
  assert(left < sizeof(primes) && "hash table size > 2147483647");
  assert(left >= 0);
  return primes[left];
}

}
