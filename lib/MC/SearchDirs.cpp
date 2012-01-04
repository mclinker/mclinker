//===- SearchDirs.cpp -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <llvm/Support/ErrorHandling.h>
#include <llvm/ADT/Twine.h>

#include "mcld/MC/SearchDirs.h"
#include "mcld/Support/FileSystem.h"
#include "mcld/MC/MCLDDirectory.h"

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
  // a magic number 8, no why.
  // please prove it or change it
  m_DirList.reserve(8);
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
  for (mcld_dir=m_DirList.begin(); mcld_dir!=mcld_dir_end; ++mcld_dir) {
    // for all entries in MCLDDirectory
    MCLDDirectory::iterator entry = (*mcld_dir)->begin();
    MCLDDirectory::iterator enEnd = (*mcld_dir)->end();

    switch(pType) {
      case Input::DynObj: {
        while (entry!=enEnd) {
          if (file == entry.path()->stem().native() ) {
            if(mcld::sys::fs::detail::shared_library_extension == entry.path()->extension().native()) {
              return entry.path();
            }
          }

          ++entry;
        }
      }

      case Input::Archive : {
        entry = (*mcld_dir)->begin();
        enEnd = (*mcld_dir)->end();
        while ( entry!=enEnd ) {
          if (file == entry.path()->stem().native() &&
            mcld::sys::fs::detail::static_library_extension == entry.path()->extension().native()) {
            return entry.path();
          }
          ++entry;
       }
     }
     default: {
       llvm::report_fatal_error(llvm::Twine("SearchDir can not recoginize namespec: `") +
                                pNamespec +
                                llvm::Twine("'."));
     }
    }
  }
  return 0;
}

