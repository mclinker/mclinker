//===- Module.cpp ---------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Module.h>
#include <mcld/LD/StaticResolver.h>
#include <mcld/LD/NamePool.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// Module
//===----------------------------------------------------------------------===//
Module::Module()
  : m_Type(Unknown), m_NamePool(1024) {
}

Module::Module(const std::string& pName, Module::Type pType)
  : m_Type(pType), m_Name(pName), m_NamePool(1024) {
}
                  
Module::~Module()
{
}

