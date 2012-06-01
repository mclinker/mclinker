//===- CIE.cpp ------------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/CIE.h>

using namespace mcld;

//==========================
// CIE

CIE::CIE(MCRegionFragment& pFrag)
  : m_Fragment(pFrag) {
}

CIE::~CIE()
{
}

uint32_t CIE::length() const
{
  return 0;
}

uint64_t CIE::extendedLength() const
{
  return 0;
}

uint32_t CIE::id() const
{
  return 0;
}

uint8_t CIE::version() const
{
  return 0;
}

const char* CIE::augString() const
{
  return 0;
}

uint32_t CIE::codeAlignFactor() const
{
  return 0;
}

uint32_t CIE::dataAlignFactor() const
{
  return 0;
}

uint8_t CIE::returnAddressReg() const
{
  return 0;
}

uint32_t CIE::augLength() const
{
  return 0;
}

uint8_t CIE::getFDEEncode() const
{
  return 0;
}

uint32_t CIE::getPerOffset() const
{
  return 0;
}

const char* CIE::getPerName() const
{
  return 0;
}
