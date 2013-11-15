//===- GarbageCollection.cpp ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/GarbageCollection.h>

using namespace mcld;

//===----------------------------------------------------------------------===//
// GarbageCollection
//===----------------------------------------------------------------------===//
GarbageCollection::GarbageCollection(const LinkerConfig& pConfig,
                                     Module& pModule)
  : m_Config(pConfig), m_Module(pModule)
{
}

GarbageCollection::~GarbageCollection()
{
}

bool run()
{
  return true;
}
