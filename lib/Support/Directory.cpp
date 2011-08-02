/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   CSMON <chun-hung.lu@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/Directory.h>
#include <mcld/Support/Path.h>

using namespace mcld;
using namespace mcld::sys::fs;

//==========================
// Directory

Directory& Directory::operator=(const Directory& pCopy)
{
}


void Directory::assign(const sys::fs::Path& pPath, 
                       FileStatus st,
                       FileStatus symlink_st)
{
}


const sys::fs::Path& Directory::path() const
{
}

FileStatus Directory::status() const
{
}

FileStatus Directory::symlinkStatus() const
{
}

