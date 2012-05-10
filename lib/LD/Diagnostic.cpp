//===- Diagnostic.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/Diagnostic.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
//  Diagnostic
Diagnostic::Diagnostic(DiagnosticEngine& pEngine)
  : m_Engine(pEngine) {
}

Diagnostic::~Diagnostic()
{
}

// format - format this diagnostic into string, subsituting the formal
// arguments. The result is appended at on the pOutStr.
void Diagnostic::format(std::string& pOutStr) const
{
  pOutStr.append("formating");
}

// format - format the given formal string, subsituting the formal
// arguments. The result is appended at on the pOutStr.
void Diagnostic::format(const char* pBegin, const char* pEnd,
                        std::string& pOutStr) const
{
  pOutStr.append("formating");
}

