//===- DiagnosticEngine.cpp -----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/DiagnosticEngine.h>
#include <mcld/LD/DiagnosticPrinter.h>
#include <mcld/LD/DiagnosticLineInfo.h>
#include <mcld/MC/MCLDInfo.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// DiagnosticEngine
//===----------------------------------------------------------------------===//
DiagnosticEngine::DiagnosticEngine()
  : m_pLDInfo(NULL), m_pLineInfo(NULL), m_pPrinter(NULL),
    m_pInfoMap(NULL), m_OwnPrinter(false) {
}

DiagnosticEngine::~DiagnosticEngine()
{
  if (m_OwnPrinter && m_pPrinter != NULL)
    delete m_pPrinter;
}

void DiagnosticEngine::reset(const MCLDInfo& pLDInfo)
{
  m_pLDInfo = &pLDInfo;
  m_pInfoMap = new DiagnosticInfos(*m_pLDInfo);
  m_State.reset();
}

void DiagnosticEngine::setLineInfo(DiagnosticLineInfo& pLineInfo)
{
  m_pLineInfo = &pLineInfo;
}

void DiagnosticEngine::setPrinter(DiagnosticPrinter& pPrinter,
                                  bool pShouldOwnPrinter)
{
  if (m_OwnPrinter && NULL != m_pPrinter)
    delete m_pPrinter;
  m_pPrinter = &pPrinter;
  m_OwnPrinter = pShouldOwnPrinter;
}

// emit - process current diagnostic.
bool DiagnosticEngine::emit()
{
  bool emitted = m_pInfoMap->process(*this);
  m_State.reset();
  return emitted;
}

