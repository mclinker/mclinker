//===- Diagnostic.h -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_DIAGNOSTIC_H
#define MCLD_DIAGNOSTIC_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <string>
#include <cassert>
#include <mcld/LD/DiagnosticEngine.h>

namespace mcld
{

/** \class Diagnostic
 *  \brief Diagnostic provides current status to DiagnosticPrinters.
 */
class Diagnostic
{
public:
  Diagnostic(DiagnosticEngine& pEngine);

  ~Diagnostic();

  unsigned int getID() const
  { return m_Engine.state().ID; }

  unsigned int getNumArgs() const
  { return m_Engine.state().numArgs; }

  DiagnosticEngine::ArgumentKind getArgKind(unsigned int pIdx) const {
    assert(pIdx < getNumArgs() && "Argument index is out of range!");
    return (DiagnosticEngine::ArgumentKind)m_Engine.state().ArgumentKinds[pIdx];
  }

  const std::string &getArgStdStr(unsigned int pIdx) const {
    assert(getArgKind(pIdx) == DiagnosticEngine::ak_std_string &&
           "Invalid argument accessor!");
    return m_Engine.state().ArgumentStrs[pIdx];
  }

  const char* getArgCStr(unsigned int pIdx) const {
    assert(getArgKind(pIdx) == DiagnosticEngine::ak_c_string &&
           "Invalid argument accessor!");
    return reinterpret_cast<const char*>(m_Engine.state().ArgumentVals[pIdx]);
  }

  int getArgSInt(unsigned int pIdx) const {
    assert(getArgKind(pIdx) == DiagnosticEngine::ak_sint &&
           "Invalid argument accessor!");
    return (int)m_Engine.state().ArgumentVals[pIdx];
  }

  unsigned int getArgUInt(unsigned int pIdx) const {
    assert(getArgKind(pIdx) == DiagnosticEngine::ak_uint &&
           "Invalid argument accessor!");
    return (unsigned int)m_Engine.state().ArgumentVals[pIdx];
  }

  bool getArgBool(unsigned int pIdx) const {
    assert(getArgKind(pIdx) == DiagnosticEngine::ak_bool &&
           "Invalid argument accessor!");
    return (bool)m_Engine.state().ArgumentVals[pIdx];
  }

  intptr_t getRawVals(unsigned int pIdx) const {
    assert(getArgKind(pIdx) != DiagnosticEngine::ak_std_string &&
           "Invalid argument accessor!");
    return m_Engine.state().ArgumentVals[pIdx];
  }

  // format - format this diagnostic into string, subsituting the formal
  // arguments. The result is appended at on the pOutStr.
  void format(std::string& pOutStr) const;

  // format - format the given formal string, subsituting the formal
  // arguments. The result is appended at on the pOutStr.
  void format(const char* pBegin, const char* pEnd, std::string& pOutStr) const;

private:
  DiagnosticEngine& m_Engine;
};

} // namespace of mcld

#endif

