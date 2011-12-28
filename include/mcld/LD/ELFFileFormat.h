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

  // -----  access functions  ----- //
  LDSection& getNullSection() {
    assert(NULL != f_pELFNULLSection);
    return *f_pELFNULLSection;
  }

  const LDSection& getNullSection() const {
    assert(NULL != f_pELFNULLSection);
    return *f_pELFNULLSection;
  }

  LDSection& getSymTab() {
    assert(NULL != f_pELFSymTab);
    return *f_pELFSymTab;
  }

  const LDSection& getSymTab() const {
    assert(NULL != f_pELFSymTab);
    return *f_pELFSymTab;
  }

  LDSection& getStrTab() {
    assert(NULL != f_pELFStrTab);
    return *f_pELFStrTab;
  }

  const LDSection& getStrTab() const {
    assert(NULL != f_pELFStrTab);
    return *f_pELFStrTab;
  }

  LDSection& getComment() {
    assert(NULL != f_pELFComment);
    return *f_pELFComment;
  }

  const LDSection& getComment() const {
    assert(NULL != f_pELFComment);
    return *f_pELFComment;
  }

protected:
  //         variable name         :  ELF
  LDSection* f_pELFNULLSection;
  LDSection* f_pELFSymTab;         // .symtab
  LDSection* f_pELFStrTab;         // .strtab
  LDSection* f_pELFComment;        // .comment
};

} // namespace of mcld

#endif

