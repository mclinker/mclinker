//===- LDFileFormat.h -----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_FILE_FORMAT_H
#define MCLD_ELF_FILE_FORMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/LDFileFormat.h>

namespace mcld
{

class MCLinker;
class LDSection;

/** \class ELFFileFormat
 *  \brief ELFFileFormat describes the common file formats in ELF.
 */
class ELFFileFormat : public LDFileFormat
{
public:
  ELFFileFormat();

  virtual ~ELFFileFormat();

  virtual void initObjectFormat(MCLinker& pLinker);

  virtual void initObjectType(MCLinker& pLinker) = 0;

protected:
  //         variable name         :  ELF
  LDSection* f_pELFNULLSection;
  LDSection* f_pELFSymTab;         // .symtab
  LDSection* f_pELFStrTab;         // .strtab
  LDSection* f_pELFComment;        // .comment
  LDSection* f_pELFSectStrTab;     // .shstrtab
};

} // namespace of mcld

#endif

