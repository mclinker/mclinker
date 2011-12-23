//===- ELFObjectReader.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFObjectReader.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/Target/TargetLDBackend.h"
#include "mcld/MC/MCRegionFragment.h"
#include "mcld/Support/rslinker/ELFSectionHeaderTable.h"
#include "mcld/Support/rslinker/ELFSectionRelTable.h"

#include <string>

using namespace llvm;
using namespace mcld;

//==========================
// ELFObjectReader
ELFObjectReader::ELFObjectReader(TargetLDBackend& pBackend, MCLinker& pLinker)
  : ObjectReader(pBackend),
    ELFReader(),
    m_Linker(pLinker)
{
}

ELFObjectReader::~ELFObjectReader()
{
}

bool ELFObjectReader::isMyFormat(Input &pFile) const
{
  // TODO: Since normalize use this function before read symbol,
  //       so we check extension first.
  // NOTE: Add ELFObject Cache, check Type and Machine.
  return pFile.path().extension().native().find(".o") != std::string::npos;
}

LDReader::Endian ELFObjectReader::endian(Input &pFile) const
{
  if (isLittleEndian(pFile))
    return LDReader::LittleEndian;
  return LDReader::BigEndian;
}

llvm::error_code ELFObjectReader::readObject(Input& pFile)
{
  // TODO
  return llvm::error_code();
}

bool ELFObjectReader::readSections(Input& pFile)
{
  std::auto_ptr<ELFObject<32> > object =
    ELFReader::createELFObject(pFile);

  const ELFSectionHeaderTable<32> *shtab =
    object->getSectionHeaderTable();

  for (size_t i = 1; i < object->getSectionNumber(); ++i) {

    const ELFSectionHeader<32> *sh = (*shtab)[i];
#if 0
    llvm::errs() << "Reading Section:" << sh->getName() << " type: " << (int) sh->getType() << "\n";
#endif

    if (sh->getType() != SHT_PROGBITS && sh->getType() != SHT_NOBITS) {
      continue;
    }

    // FIXME: Skip debug section.
    if (std::string(sh->getName()).find(".debug") != std::string::npos) {
      continue;
    }

#if 0
    llvm::errs() << "Reading Section:" << sh->getName() << " Start: 0x" ;
    llvm::errs().write_hex(sh->getOffset());
    llvm::errs() << " Length: 0x" ;
    llvm::errs().write_hex(sh->getSize());
    llvm::errs() << "\n";
#endif

    LDSection* ldSect = m_Linker.createSectHdr(sh->getName(), ELFReader::getLDSectionKind(sh->getName()), sh->getType(), sh->getFlags());
    llvm::MCSectionData* mcSectData = m_Linker.getOrCreateSectData(ldSect);
    MCFragment *mcFrag = NULL;
    if (sh->getType() == SHT_PROGBITS)
      mcFrag = new MCRegionFragment(*pFile.memArea()->request(sh->getOffset(),sh->getSize()),mcSectData);
    else
      mcFrag = new MCFillFragment(0,1,sh->getSize());

    if (!mcFrag)
      llvm::report_fatal_error("MCFragment can't be NULL.");

    mcSectData->getFragmentList().push_back(mcFrag);
  }

  return true;
}

bool ELFObjectReader::readSymbols(Input& pFile)
{
  std::auto_ptr<ELFObject<32> > object =
    ELFReader::createELFObject(pFile);

  ELFSectionSymTab<32>* symtab =
    static_cast<ELFSectionSymTab<32> *>(object->getSectionByName(".symtab"));

  // Skip Start NULL Symbol
  for (size_t i = 1 ; i < symtab->size() ; i++) {
    ELFSymbol<32>* sym = (*symtab)[i];

#if 0
    llvm::errs () << "ADD Symbol: " << (*symtab)[i]->getName(false) << " Binding: ";
    llvm::errs () << (int)(*symtab)[i]->getBindingAttribute();
    llvm::errs () << " Type: " << (int)(*symtab)[i]->getType();
    llvm::errs () << " Visibility: " << (int)(*symtab)[i]->getVisibility() << ".\n";
#endif

    int stt_type = (int)sym->getType();

    // Only handle NOTYPE, OBJECT and FUNC
    if ( stt_type >= 3 ) {
      if ( stt_type >= 5) {
        llvm::errs () << "[WARNING]: STT Type " << stt_type;
        llvm::errs () << "Not implement.\n";
      }
      continue;
    }

    ResolveInfo::Binding bind = ELFReader::getBindingResolveInfo(sym, false);
    ResolveInfo::Desc desc =
      (sym->getSectionIndex() == SHN_UNDEF) ?
      ResolveInfo::Undefined : ResolveInfo::Define;

    ResolveInfo::Visibility vis = ELFReader::getVisibilityResolveInfo(sym);
    uint64_t sym_size = sym->getSize();
    uint64_t sym_value = sym->getValue();

    bool local_sym = bind == ResolveInfo::Local;

    if (local_sym) {
      m_Linker.addLocalSymbol(
        llvm::StringRef(sym->getName(false)), desc, sym_size, vis);
    }
    else {
      m_Linker.addGlobalSymbol(
        llvm::StringRef(sym->getName(false)), false, desc, bind,
        sym_size, vis
      );
    }
  }

  return true;
}

