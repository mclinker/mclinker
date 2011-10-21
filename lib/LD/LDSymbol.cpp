//===- LDSymbol.cpp -------------------------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/LDSymbol.h"

using namespace mcld;

LDSymbol::LDSymbol()
 : m_pName(0), m_IsDyn(true), m_Type(NoneType), m_Binding(NoneBinding),
   m_pSection(0), m_Value(0), m_Size(0), m_Other(0) {
}

LDSymbol::~LDSymbol()
{
}

