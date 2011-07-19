/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#ifndef SEARCHDIRS_H
#define SEARCHDIRS_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <llvm/ADT/StringRef.h>

namespace mcld
{
class MCLDFile;
/** \class SearchDirs
 *  \brief SearchDirs contains the list of paths that MCLinker will search for
 *  archive libraries and control scripts.
 *
 *  \see
 *  \author Luba Tang <lubatang@mediatek.com>
 */
class SearchDirs
{
public:
  MCLDFile* hasFile(llvm::StringRef pNamespec, std::string& pAbsPath);
  MCLDFile* hasFile(llvm::StringRef pAbsPath);
};

} // namespace of mcld

#endif

