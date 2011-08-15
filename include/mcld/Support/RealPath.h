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
#include <string>

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
  typedef Path::ValueType  ValueType;
  typedef Path::StringType StringType;

public:
  RealPath();
  explicit RealPath(const ValueType* s );
  explicit RealPath(const StringType &s );
  explicit RealPath(const Path& pPath);

  ~RealPath();
};

} // namespace of fs
} // namespace of sys
} // namespace of mcld

//-------------------------------------------------------------------------//
//                              STL compatible functions                   //
//-------------------------------------------------------------------------//
namespace std {

template<>
struct less<mcld::sys::fs::RealPath> : public binary_function<
                                                     mcld::sys::fs::RealPath,
                                                     mcld::sys::fs::RealPath,
                                                     bool>
{
  bool operator() (const mcld::sys::fs::RealPath& pX,
                   const mcld::sys::fs::RealPath& pY) const {
    if (pX.native().size() < pY.native().size())
      return true;
    return (pX.native() < pY.native());
  }
};

} // namespace of std


#endif

