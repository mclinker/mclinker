/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Reloc.h <nowar100@gmail.com>                                            *
 ****************************************************************************/
#ifndef RELOCDATA_H
#define RELOCDATA_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <vector>

namespace mcld
{

// Data about a single relocation section.  This is read in
// read_relocs and processed in scan_relocs.
struct SectionRelocData {
  //MCLDFile* contents;
};


/** \class RelocData
 *  \brief Information of relocations in an object file.
 *
 *  \see
 *  \author Reloc.h <nowar100@gmail.com>
 */
struct RelocData
{
  typedef std::vector<SectionRelocData> SectionRelocList;
  SectionRelocList relocs;
};

} // namespace of mcld

#endif

