//===- FileSystem.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/Config/Config.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/Support/Path.h>

using namespace mcld::sys::fs;

//===--------------------------------------------------------------------===//
// non-member functions
//===--------------------------------------------------------------------===//
bool mcld::sys::fs::exists(const Path &pPath)
{
  FileStatus pFileStatus;
  detail::status(pPath, pFileStatus);
  return exists(pFileStatus);
}

bool mcld::sys::fs::is_directory(const Path &pPath)
{
  FileStatus pFileStatus;
  detail::status(pPath, pFileStatus);
  return is_directory(pFileStatus);
}

// Include the truly platform-specific parts. 
#if defined(MCLD_ON_UNIX)
#include "Unix/FileSystem.inc"
#include "Unix/PathV3.inc" 
#endif 
#if defined(MCLD_ON_WIN32)
#include "Windows/FileSystem.inc"
#include "Windows/PathV3.inc" 
#endif 
