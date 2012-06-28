//===- LinkerConfig.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "alone/Support/LinkerConfig.h"

using namespace alone;

LinkerConfig::LinkerConfig(const std::string& pTriple)
  : mTriple(pTriple), mTarget(NULL) {
}

bool LinkerConfig::initializeTarget()
{
}

