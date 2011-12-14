//===- ScopedWriter.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <mcld/Support/ScopedWriter.h>

using namespace mcld;

//==========================
// ScopedRegion
ScopedWriter::ScopedWriter(MemoryRegion *pRegion, bool pSameEndian)
  : m_Region(pRegion),
    m_Buffer(pRegion->getBuffer()),
    m_SameEndian(pSameEndian),
    m_Cursor(0) {
}

