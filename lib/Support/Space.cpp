//===- Space.cpp ----------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/Space.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Space
Space::Space(Space::Type pType, void* pMemBuffer, size_t pSize)
  : m_Data(pMemBuffer), m_StartOffset(0), m_Size(pSize),
    m_RegionCount(0), m_Type(pType)
{
}

Space::~Space()
{
}

