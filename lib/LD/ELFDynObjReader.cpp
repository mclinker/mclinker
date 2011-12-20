//===- ELFDynObjReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include <mcld/LD/ELFDynObjReader.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/Target/TargetLDBackend.h>
#include <mcld/MC/MCLDInput.h>

#include <sstream>
#include <string>

using namespace mcld;

//==========================
// ELFDynObjReader
ELFDynObjReader::ELFDynObjReader(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjReader(pBackend), m_Linker(pLinker) {
}

ELFDynObjReader::~ELFDynObjReader()
{
}

LDReader::Endian ELFDynObjReader::endian(Input& pFile) const
{
  // TODO
  return LDReader::LittleEndian;
}

bool ELFDynObjReader::isMyFormat(Input &pFile) const
{
  // TODO: Since normalize use this function before read symbol,
  //       so we check extension first.
  // NOTE: Add ELFObject Cache, check Type and Machine
  return pFile.path().extension().native().find(".so") != std::string::npos;
}

llvm::error_code ELFDynObjReader::readDSO(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFDynObjReader::readSymbols(Input& pFile)
{
  std::auto_ptr<ELFObject<32> > object =
    ELFReader::createELFObject(pFile);

  ELFSectionSymTab<32>* symtab =
    static_cast<ELFSectionSymTab<32> *>(object->getSectionByName(".dynsym"));

  // Skip Start NULL Symbol
  for (size_t i = 1 ; i < symtab->size() ; i++) {
    ELFSymbol<32>* sym = (*symtab)[i];

#if 0
      llvm::errs () << "ADD Symbol: " << (*symtab)[i]->getName() << " Binding: ";
      llvm::errs () << (int)(*symtab)[i]->getBindingAttribute();
      llvm::errs () << " Type: " << (int)(*symtab)[i]->getType() << ".\n";
#endif

    /* LDSymbol* addGlobalSymbol(
         const llvm::StringRef& pName,
         bool pIsDyn,
         ResolveInfo::Desc pDesc,
         ResolveInfo::Binding pBinding,
         ResolveInfo::ValueType pValue,
         ResolveInfo::SizeType pSize,
         ResolveInfo::Visibility pVisibility = ResolveInfo::Default); */

    ResolveInfo::Binding bind = ELFReader::getBindingResolveInfo(sym, true);
    // TODO: Double check undefine logic in DSO.
    ResolveInfo::Desc desc =
      (sym->getSectionIndex() == SHN_UNDEF) ?
      ResolveInfo::Undefined : ResolveInfo::Define;

    uint64_t sym_size = sym->getSize();
    uint64_t sym_value = sym->getValue();

    m_Linker.addGlobalSymbol(
      llvm::StringRef(sym->getName()), true, desc, bind,
      sym_size, sym_value
    );
  }

  return true;
}

