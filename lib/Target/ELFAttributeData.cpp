//===- ELFAttributeData.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/ELFAttributeData.h>

#include <mcld/Support/LEB128.h>
#include <mcld/Target/ELFAttributeValue.h>

using namespace mcld;

bool ELFAttributeData::ReadTag(TagType& pTag, const char* &pBuf,
                               size_t &pBufSize)
{
  size_t size = 0;

  pTag = static_cast<ELFAttributeData::TagType>(
            leb128::decode<uint64_t>(pBuf, size));

  if (size > pBufSize)
    return false;

  pBuf += size;
  pBufSize -= size;

  return true;
}

bool ELFAttributeData::ReadValue(ELFAttributeValue& pValue, const char* &pBuf,
                                 size_t &pBufSize)
{
  // An ULEB128-encoded value
  if (pValue.isIntValue()) {
    size_t size = 0;
    uint64_t int_value = leb128::decode<uint64_t>(pBuf, size);
    pValue.setIntValue(static_cast<unsigned int>(int_value));

    if (size > pBufSize)
      return false;

    pBuf += size;
    pBufSize -= size;
  }

  // A null-terminated byte string
  if (pValue.isStringValue()) {
    pValue.setStringValue(pBuf);

    size_t size = pValue.getStringValue().length() + 1 /* '\0' */;
    assert(size <= pBufSize);
    pBuf += size;
    pBufSize -= size;
  }

  return true;
}
