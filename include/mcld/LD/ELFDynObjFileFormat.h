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

class MCLinker;
class LDSection;

/** \class ELFDynObjFileFormat
 *  \brief ELFDynObjFileFormat describes the format for ELF dynamic objects.
 */
class ELFDynObjFileFormat : public ELFFileFormat
{
public:
  void initObjectType(MCLinker& pLinker);

protected:
  //         variable name         :  ELF
  LDSection* f_pDynSymTab;         // .dynsym
  LDSection* f_pDynStrTab;         // .dynstr
  LDSection* f_pInterp;            // .interp
  LDSection* f_pHashTab;           // .hash
  LDSection* f_pDynamic;           // .dynamic
  LDSection* f_pRelaDyn;           // .rela.dyn
  LDSection* f_pRelaPlt;           // .rela.plt
  LDSection* f_pRelDyn;            // .rel.dyn
  LDSection* f_pRelPlt;            // .rel.plt
  LDSection* f_pGOT;               // .got
  LDSection* f_pPLT;               // .plt
  LDSection* f_pGOTPLT;            // .got.plt
  LDSection* f_pPreInitArray;      // .preinit_array
  LDSection* f_pInitArray;         // .init_array
  LDSection* f_pFiniArray;         // .fini_array
  LDSection* f_pCtors;             // .ctors
  LDSection* f_pDtors;             // .dtors
};

} // namespace of mcld

#endif

