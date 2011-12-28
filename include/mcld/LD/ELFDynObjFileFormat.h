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

  // -----  access functions  ----- //
  LDSection& getDynSymTab() {
    assert(NULL != f_pDynSymTab);
    return *f_pDynSymTab;
  }
  
  const LDSection& getDynSymTab() const {
    assert(NULL != f_pDynSymTab);
    return *f_pDynSymTab;
  }
  
  LDSection& getDynStrTab() {
    assert(NULL != f_pDynStrTab);
    return *f_pDynStrTab;
  }
  
  const LDSection& getDynStrTab() const {
    assert(NULL != f_pDynStrTab);
    return *f_pDynStrTab;
  }
  
  LDSection& getInterp() {
    assert(NULL != f_pInterp);
    return *f_pInterp;
  }
  
  const LDSection& getInterp() const {
    assert(NULL != f_pInterp);
    return *f_pInterp;
  }
  
  LDSection& getHashTab() {
    assert(NULL != f_pHashTab);
    return *f_pHashTab;
  }
  
  const LDSection& getHashTab() const {
    assert(NULL != f_pHashTab);
    return *f_pHashTab;
  }
  
  LDSection& getDynamic() {
    assert(NULL != f_pDynamic);
    return *f_pDynamic;
  }
  
  const LDSection& getDynamic() const {
    assert(NULL != f_pDynamic);
    return *f_pDynamic;
  }
  
  LDSection& getRelaDyn() {
    assert(NULL != f_pRelaDyn);
    return *f_pRelaDyn;
  }
  
  const LDSection& getRelaDyn() const {
    assert(NULL != f_pRelaDyn);
    return *f_pRelaDyn;
  }
  
  LDSection& getRelaPlt() {
    assert(NULL != f_pRelaPlt);
    return *f_pRelaPlt;
  }
  
  const LDSection& getRelaPlt() const {
    assert(NULL != f_pRelaPlt);
    return *f_pRelaPlt;
  }
  
  LDSection& getRelDyn() {
    assert(NULL != f_pRelDyn);
    return *f_pRelDyn;
  }
  
  const LDSection& getRelDyn() const {
    assert(NULL != f_pRelDyn);
    return *f_pRelDyn;
  }
  
  LDSection& getRelPlt() {
    assert(NULL != f_pRelPlt);
    return *f_pRelPlt;
  }
  
  const LDSection& getRelPlt() const {
    assert(NULL != f_pRelPlt);
    return *f_pRelPlt;
  }
  
  LDSection& getGOT() {
    assert(NULL != f_pGOT);
    return *f_pGOT;
  }
  
  const LDSection& getGOT() const {
    assert(NULL != f_pGOT);
    return *f_pGOT;
  }
  
  LDSection& getPLT() {
    assert(NULL != f_pPLT);
    return *f_pPLT;
  }
  
  const LDSection& getPLT() const {
    assert(NULL != f_pPLT);
    return *f_pPLT;
  }
  
  LDSection& getGOTPLT() {
    assert(NULL != f_pGOTPLT);
    return *f_pGOTPLT;
  }
  
  const LDSection& getGOTPLT() const {
    assert(NULL != f_pGOTPLT);
    return *f_pGOTPLT;
  }
  
  LDSection& getPreInitArray() {
    assert(NULL != f_pPreInitArray);
    return *f_pPreInitArray;
  }
  
  const LDSection& getPreInitArray() const {
    assert(NULL != f_pPreInitArray);
    return *f_pPreInitArray;
  }
  
  LDSection& getInitArray() {
    assert(NULL != f_pInitArray);
    return *f_pInitArray;
  }
  
  const LDSection& getInitArray() const {
    assert(NULL != f_pInitArray);
    return *f_pInitArray;
  }
  
  LDSection& getFiniArray() {
    assert(NULL != f_pFiniArray);
    return *f_pFiniArray;
  }
  
  const LDSection& getFiniArray() const {
    assert(NULL != f_pFiniArray);
    return *f_pFiniArray;
  }
  
  LDSection& getCtors() {
    assert(NULL != f_pCtors);
    return *f_pCtors;
  }
  
  const LDSection& getCtors() const {
    assert(NULL != f_pCtors);
    return *f_pCtors;
  }
  
  LDSection& getDtors() {
    assert(NULL != f_pDtors);
    return *f_pDtors;
  }
  
  const LDSection& getDtors() const {
    assert(NULL != f_pDtors);
    return *f_pDtors;
  }

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

