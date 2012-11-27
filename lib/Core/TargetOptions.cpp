//===- TargetOptions.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/TargetOptions.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// TargetOptions
//===----------------------------------------------------------------------===//
TargetOptions::TargetOptions()
{
}

TargetOptions::TargetOptions(const std::string& pTriple)
  : m_Triple(pTriple) {
}

TargetOptions::~TargetOptions()
{
}

void TargetOptions::setTriple(const llvm::Triple& pTriple)
{
  m_Triple = pTriple;
}

void TargetOptions::setTriple(const std::string& pTriple)
{
  m_Triple.setTriple(pTriple);
}

