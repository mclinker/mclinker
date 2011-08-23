/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#include <mcld/Support/RealPath.h>
#include <mcld/Support/FileSystem.h>

#ifdef MCLD_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace mcld::sys::fs;

//==========================
// RealPath
RealPath::RealPath()
  : Path() {
}

RealPath::RealPath(const RealPath::ValueType* s )
  : Path(s) {
  initialize();
}

RealPath::RealPath(const RealPath::StringType &s )
  : Path(s) {
  initialize();
}

RealPath::RealPath(const Path& pPath)
 : Path(pPath) {
  initialize();
}

RealPath::~RealPath()
{
}

RealPath& RealPath::assign(const Path& pPath)
{
  Path::m_PathName.assign(pPath.native());
}

void RealPath::initialize()
{
  if (isFromRoot()) {
    detail::canonicalize(m_PathName);
  }
  else if (isFromPWD()) {
    std::string path_name;
    detail::get_pwd(path_name);
    path_name += '/';
    path_name += m_PathName;
    cerr << "before can=" << path_name << endl;
    detail::canonicalize(path_name);
    cerr << "after can=" << path_name << endl;
    m_PathName = path_name;
  }
}

