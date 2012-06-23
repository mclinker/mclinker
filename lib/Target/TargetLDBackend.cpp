//===- TargetLDBackend.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/Relocation.h"
#include "mcld/LD/Layout.h"
#include "mcld/Target/TargetLDBackend.h"

using namespace mcld;

/* ** */
TargetLDBackend::TargetLDBackend()
  : m_pEhFrame(NULL) {
}

TargetLDBackend::~TargetLDBackend()
{
  if (NULL != m_pEhFrame)
    delete m_pEhFrame;
}

EhFrame* TargetLDBackend::getEhFrame()
{
  if (NULL == m_pEhFrame)
    m_pEhFrame = new EhFrame();
  return m_pEhFrame;
}

const EhFrame* TargetLDBackend::getEhFrame() const
{
  assert(NULL == m_pEhFrame);
  return m_pEhFrame;
}
