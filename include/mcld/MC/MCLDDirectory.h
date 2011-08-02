/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Chun-Hung Lu <chun-hung.lu@mediatek.com>                                *
 ****************************************************************************/
#ifndef MCLDDIRECTORY_H
#define MCLDDIRECTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/Support/Directory.h>
#include <mcld/Support/FileSystem.h>
#include <llvm/ADT/StringRef.h>
#include <string>

namespace mcld
{

/** \class MCLDDirectory
 *  \brief MCLDDirectory is an directory entry for library search.
 *
 */
class MCLDDirectory : public sys::fs::Directory
{
public:
  MCLDDirectory();
  MCLDDirectory(const std::string& pName);
  MCLDDirectory(llvm::StringRef pName);
  ~MCLDDirectory();

public:
  MCLDDirectory &assign(llvm::StringRef pName);
  bool isInSysroot() const;
  const sys::fs::Path &path() const;
  llvm::StringRef name() const;

  /// setSysroot - if MCLDDirectory is in sysroot, modify the path.
  void setSysroot(const sys::fs::Path& pPath);

private:
  std::string m_Name;
  bool m_bInSysroot;
};

} // namespace of mcld

#endif

