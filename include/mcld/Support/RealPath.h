/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#ifndef MCLD_REAL_PATH_H
#define MCLD_REAL_PATH_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/Path.h>

namespace mcld {
namespace sys  {
namespace fs   {

/** \class RealPath
 *  \brief The canonicalized absolute pathname.
 *
 */
class RealPath : public Path
{
public:
  Path::ValueType  ValueType;
  Path::StringType StringType;

public:
  RealPath();
  explicit RealPath(const std::string& pPath);
  explicit RealPath(const ValueType* s );
  explicit RealPath(const StringType &s );
  explicit RealPath(const Path& pPath);

  ~RealPath();
};

} // namespace of fs
} // namespace of sys
} // namespace of mcld

#endif

