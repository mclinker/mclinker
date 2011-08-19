/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/Path.h>

using namespace mcld::sys::fs;


//===--------------------------------------------------------------------===//
// SearchDirs

//===--------------------------------------------------------------------===//
// non-member functions

// Include the truly platform-specific parts. 
#if defined(LLVM_ON_UNIX) 
#include "Unix/FileSystem.inc"
#include "Unix/PathV3.inc" 
#endif 
#if defined(LLVM_ON_WIN32) 
#include "Windows/FileSystem.inc"
#include "Windows/PathV3.inc" 
#endif 
