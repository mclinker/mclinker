//===- ELFDynObjFileFormat.h ----------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_ELF_DYNAMIC_OBJECT_FILE_FROMAT_H
#define MCLD_ELF_DYNAMIC_OBJECT_FILE_FROMAT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif
#include <mcld/LD/ELFFileFormat.h>

namespace mcld
{

class LDContext;
class MCLinker;
class LDSection;

/** \class ELFDynObjFileFormat
 *  \brief ELFDynObjFileFormat describes the format for ELF dynamic objects.
 */
class ELFDynObjFileFormat : public ELFFileFormat
{
public:
  void initObjectType(MCLinker& pLinker, LDContext& pContext);

protected:
  LDSection* f_pDynSymTab;
  LDSection* f_pDynStrTab;
  LDSection* f_pInterp;
  LDSection* f_pHashTab;
  LDSection* f_pDynamic;
  LDSection* f_pRelaDyn;
  LDSection* f_pRelaPlt;
  LDSection* f_pRelDyn;
  LDSection* f_pRelPlt;
  LDSection* f_pGOT;
  LDSection* f_pPLT;
  LDSection* f_pGOTPLT;
};

} // namespace of mcld

#endif

