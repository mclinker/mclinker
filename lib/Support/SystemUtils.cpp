//===- SystemUtils.cpp ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Support/SystemUtils.h>

using namespace mcld::sys;

//===----------------------------------------------------------------------===//
// Non-member functions
#if defined(LLVM_ON_UNIX)
#include "Unix/System.inc"
#endif
#if defined(LLVM_ON_WIN32) 
#include "Windows/System.inc"
#endif 
