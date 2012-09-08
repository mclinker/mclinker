//===- TargetLDBackend.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/LD/EhFrame.h>
#include <mcld/LD/Layout.h>
#include <mcld/LinkerConfig.h>

using namespace mcld;

TargetLDBackend::TargetLDBackend(const LinkerConfig& pConfig)
  : m_pEhFrame(NULL), m_Config(pConfig) {
}

TargetLDBackend::~TargetLDBackend()
{
  if (NULL != m_pEhFrame)
    delete m_pEhFrame;
}

EhFrame* TargetLDBackend::getEhFrame()
{
  if (NULL == m_pEhFrame)
    m_pEhFrame = new EhFrame(isLittleEndian());
  return m_pEhFrame;
}

const EhFrame* TargetLDBackend::getEhFrame() const
{
  assert(NULL == m_pEhFrame);
  return m_pEhFrame;
}
