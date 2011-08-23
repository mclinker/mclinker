/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#include <mcld/Support/RealPath.h>
#include <mcld/Support/FileSystem.h>

using namespace mcld::sys::fs;

//==========================
// RealPath
RealPath::RealPath()
  : Path() {
}

RealPath::RealPath(const RealPath::ValueType* s )
  : Path(s) {
  detail::canonical_form(*this);
}

RealPath::RealPath(const RealPath::StringType &s )
  : Path(s) {
  detail::canonical_form(*this);
}

RealPath::RealPath(const Path& pPath)
 : Path(pPath) {
  detail::canonical_form(*this);
}

RealPath::~RealPath()
{
}

RealPath& RealPath::assign(const Path& pPath)
{
  Path::m_PathName.assign(pPath.native());
}

