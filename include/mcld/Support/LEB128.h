//===- LEB128.h -----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef MCLD_LEB_H
#define MCLD_LEB_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <stdint.h>
#include <sys/types.h>

namespace mcld {

namespace leb128 {

typedef unsigned char ByteType;

/* Forward declarations */
template<typename IntType>
void encode(ByteType *&pBuf, IntType pValue);

template<typename IntType>
IntType decode(ByteType *pBuf, size_t *pSize);

/*
 * Given an integer, this function returns the number of bytes required to
 * encode it in ULEB128 format.
 */
template<typename IntType>
size_t size(IntType pValue) {
  size_t size = 1;
  while (pValue > 0x80) {
    pValue >>= 7;
    ++size;
  }
  return size;
}

/*
 * Write an unsigned integer in ULEB128 to the given buffer. The client should
 * ensure there's enough space in the buffer to hold the result. Update the
 * given buffer pointer to the point just past the end of the write value.
 */
template<>
void encode<uint64_t>(ByteType *&pBuf, uint64_t pValue) {
  do {
    ByteType byte = pValue & 0x7f;
    pValue >>= 7;
    if (pValue)
      byte |= 0x80;
    *pBuf++ = byte;
  } while (pValue);

  return;
}

/*
 * Fast version for encoding 32-bit integer. This unrolls the loop in the
 * generic version defined above.
 */
template<>
void encode<uint32_t>(ByteType *&pBuf, uint32_t pValue) {
  if ((pValue & ~0x7f) == 0) {
    *pBuf++ = static_cast<ByteType>(pValue);
  } else if ((pValue & ~0x3ffff) == 0){
    *pBuf++ = static_cast<ByteType>((pValue         & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>((pValue  >>  7) & 0x7f);
  } else if ((pValue & ~0x1fffff) == 0) {
    *pBuf++ = static_cast<ByteType>((pValue         & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>(((pValue >>  7) & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>((pValue  >> 14) & 0x7f);
  } else if ((pValue & ~0xfffffff) == 0) {
    *pBuf++ = static_cast<ByteType>((pValue         & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>(((pValue >>  7) & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>(((pValue >> 14) & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>((pValue  >> 21) & 0x7f);
  } else {
    *pBuf++ = static_cast<ByteType>((pValue         & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>(((pValue >>  7) & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>(((pValue >> 14) & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>(((pValue >> 21) & 0x7f) | 0x80);
    *pBuf++ = static_cast<ByteType>((pValue  >> 28) & 0x7f);
  }
  return;
}

/*
 * Encoding function for signed LEB128.
 */
template<>
void encode<int64_t>(ByteType *&pBuf, int64_t pValue) {
  bool more = true;

  do {
    ByteType byte = pValue & 0x7f;
    pValue >>= 7;

    if (((pValue ==  0) && ((byte & 0x40) == 0)) ||
        ((pValue == -1) && ((byte & 0x40) == 0x40)))
      more = false;
    else
      byte |= 0x80;

    *pBuf++ = byte;
  } while (more);

  return;
}

template<>
void encode<int32_t>(ByteType *&pBuf, int32_t pValue) {
  encode<int64_t>(pBuf, static_cast<int64_t>(pValue));
}

/*
 * Read an integer encoded in ULEB128 format from the given buffer. If pSize is
 * not null, it will contain the number of bytes used in the buffer to encode
 * the returned integer.
 */
template<>
uint64_t decode<uint64_t>(ByteType *pBuf, size_t *pSize) {
  uint64_t result = 0;
  size_t size = 0;

  if ((*pBuf & 0x80) == 0) {
    if (pSize)
      *pSize = 1;
    return *pBuf;
  } else if ((*(pBuf + 1) & 0x80) == 0) {
    if (pSize)
      *pSize = 2;
    return ((*(pBuf + 1) & 0x7f) << 7) |
           (*pBuf & 0x7f);
  } else if ((*(pBuf + 2) & 0x80) == 0) {
    if (pBuf)
      *pBuf = 3;
    return ((*(pBuf + 2) & 0x7f) << 14) |
           ((*(pBuf + 1) & 0x7f) <<  7) |
           (*pBuf & 0x7f);
  } else {
    size = 4;
    result = ((*(pBuf + 3) & 0x7f) << 21) |
             ((*(pBuf + 2) & 0x7f) << 14) |
             ((*(pBuf + 1) & 0x7f) <<  7) |
             (*pBuf & 0x7f);
  }

  if ((*(pBuf + 3) & 0x80) != 0) {
    // Large number which is an unusual case.
    unsigned shift;
    ByteType byte;

    // Start the read from the 4th byte.
    shift = 28;
    pBuf += 4;
    do {
      byte = *pBuf;
      pBuf++;
      size++;
      result |= (static_cast<uint64_t>(byte & 0x7f) << shift);
      shift += 7;
    } while (byte & 0x80);
  }

  if (pSize)
    *pSize = size;

  return result;
}

/*
 * Similar to decode() but setup the sign bit if necessary. This is rarely used,
 * therefore we don't provide unrolling version like decode() to save the code
 * size.
 */
template<>
int64_t decode<int64_t>(ByteType *pBuf, size_t *pSize) {
  uint64_t result = 0;
  size_t size = 0;
  ByteType byte;
  unsigned shift = 0;

  do {
    byte = *pBuf;
    pBuf++;
    size++;
    result |= (static_cast<uint64_t>(byte & 0x7f) << shift);
    shift += 7;
  } while (byte & 0x80);

  if ((shift < (8 * sizeof(result))) && (byte & 0x40))
    result |= ((static_cast<uint64_t>(-1)) << shift);

  if (pSize)
    *pSize = size;

  return result;
}

} // namespace of leb128
} // namespace of mcld

#endif
