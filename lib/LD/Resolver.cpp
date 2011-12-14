//===- Resolver.cpp -------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/Resolver.h"
#include <cassert>

using namespace mcld;

//==========================
// Resolver
Resolver::Resolver()
  : m_Mesg() {
}

Resolver::Resolver(const Resolver& pCopy)
  : m_Mesg(pCopy.m_Mesg) {
}

Resolver::~Resolver()
{
  m_Mesg.clear();
}

void Resolver::clearMesg()
{
  m_Mesg.clear();
}

