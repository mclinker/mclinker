/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Luba Tang <lubatang@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/Support/SearchDirs.h>
#include <mcld/Support/FileSystem.h>
#include <mcld/MC/MCLDDirectory.h>

using namespace mcld;

//==========================
// Non-member functions
static void SpecToFilename(const std::string& pSpec, std::string& pFile)
{
  pFile = "lib";
  pFile += pSpec;
}

//==========================
// SearchDirs
SearchDirs::SearchDirs()
{
}

SearchDirs::~SearchDirs()
{
  iterator dir, dirEnd = end(); 
  for (dir = begin(); dir!=dirEnd; ++dir) {
    delete (*dir);
  }
}

void SearchDirs::add(const MCLDDirectory& pDirectory)
{
  m_DirList.push_back(new MCLDDirectory(pDirectory));
}

mcld::sys::fs::Path* SearchDirs::find(const std::string& pNamespec, mcld::Input::Type pType)
{
  std::string file;
  SpecToFilename(pNamespec, file);
  // for all MCLDDirectorys
  DirList::iterator mcld_dir, mcld_dir_end = m_DirList.end();
  MCLDDirectory::iterator entry, enEnd;
  for (mcld_dir=m_DirList.begin(); mcld_dir!=mcld_dir_end; ++mcld_dir) {
    // for all entries in MCLDDirectory
    enEnd = (*mcld_dir)->end();
    for (entry = (*mcld_dir)->begin(); entry!=enEnd; ++entry) {
      if (file == entry.path()->stem()) {
        switch(pType) {
        case mcld::Input::Archive:
            if (mcld::sys::fs::detail::static_library_extension == entry.path()->extension())
            return entry.path();
        case mcld::Input::DynObj:
            if (mcld::sys::fs::detail::shared_library_extension == entry.path()->extension())
            return entry.path();
        }
      }
    }
  }
  return 0;
}

