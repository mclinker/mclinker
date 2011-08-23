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
    char* prefix = get_current_dir_name();
    size_t prefix_len = strlen(prefix);
    std::string path_name;
    path_name.reserve(prefix_len+m_PathName.size()+2);
    path_name += prefix;
    path_name += '/';
    path_name += m_PathName;
    detail::canonicalize(path_name);
    m_PathName = path_name;
  }
}

