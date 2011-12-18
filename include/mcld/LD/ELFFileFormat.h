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
class LDContext;
class LDSection;

/** \class ELFFileFormat
 *  \brief ELFFileFormat describes the common file formats in ELF.
 */
class ELFFileFormat : public LDFileFormat
{
public:
  ELFFileFormat();

  virtual ~ELFFileFormat();

  virtual void initObjectFormat(MCLinker& pLinker, LDContext& pContext);

  virtual void initObjectType(MCLinker& pLinker, LDContext& pContext) = 0;

protected:
  LDSection* f_pELFNULLSection;
  LDSection* f_pELFSymTab;
  LDSection* f_pELFStrTab;
  LDSection* f_pELFComment;
  LDSection* f_pELFSectStrTab;
};

} // namespace of mcld

#endif

