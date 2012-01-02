//===- MipsLDBackend.cpp --------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <llvm/ADT/Triple.h>
#include <llvm/Support/ELF.h>
#include <mcld/Support/TargetRegistry.h>
#include <mcld/MC/MCLinker.h>
#include <mcld/LD/SectionMap.h>
#include "Mips.h"
#include "MipsDynRelSection.h"
#include "MipsLDBackend.h"
#include "MipsRelocationFactory.h"

using namespace llvm;
using namespace mcld;

MipsGNULDBackend::MipsGNULDBackend()
  : m_pRelocFactory(0) {
}

MipsGNULDBackend::~MipsGNULDBackend()
{
  if (0 != m_pRelocFactory)
    delete m_pRelocFactory;
}

RelocationFactory* MipsGNULDBackend::getRelocFactory()
{
  if (NULL == m_pRelocFactory)
    m_pRelocFactory = new MipsRelocationFactory(1024, *this);
  return m_pRelocFactory;
}

uint32_t MipsGNULDBackend::machine() const
{
  return ELF::EM_MIPS;
}

bool MipsGNULDBackend::isLittleEndian() const
{
  // Now we support little endian (mipsel) target only.
  return true;
}

unsigned int MipsGNULDBackend::bitclass() const
{
  return 32;
}

bool MipsGNULDBackend::initTargetSectionMap(SectionMap& pSectionMap)
{
  if (!pSectionMap.push_back(".MIPS.stubs", ".MIPS.stubs"))
    return false;

  return true;
}

void MipsGNULDBackend::initTargetSections(MCLinker& pLinker)
{
  // add target dependent sections here.
}

void MipsGNULDBackend::initTargetSymbols(MCLinker& pLinker)
{
  // Create symbol _GLOBAL_OFFSET_TABLE_ to mark .got section.
  if (m_pGOT.get()) {
    pLinker.defineSymbol(llvm::StringRef("_GLOBAL_OFFSET_TABLE_"),
                         false,
                         ResolveInfo::Object,
                         ResolveInfo::Define,
                         ResolveInfo::Local,
                         m_pGOT->getEntrySize(),
                         *(new MCFragmentRef(*(m_pGOT->begin()))),
                         ResolveInfo::Hidden);
  }
}

void MipsGNULDBackend::scanRelocation(Relocation& pReloc,
                                      MCLinker& pLinker,
                                      unsigned int pType)
{
  ResolveInfo* rsym = pReloc.symInfo();

  if (rsym->isLocal())
    scanLocalRelocation(pReloc, pLinker, pType);
  else if (rsym->isGlobal())
    scanGlobalRelocation(pReloc, pLinker, pType);
}

void MipsGNULDBackend::scanLocalRelocation(Relocation& pReloc,
                                           MCLinker& pLinker,
                                           unsigned int pType)
{
  ResolveInfo* rsym = pReloc.symInfo();

  switch (pReloc.type()){
    case ELF::R_MIPS_NONE:
    case ELF::R_MIPS_16:
      break;
    case ELF::R_MIPS_32:
      if (Output::DynObj == pType) {
        // TODO: (simon) Check section flag SHF_EXECINSTR
        // half_t shndx = rsym->getSectionIndex();
        if (true) {
          if (NULL == m_pRelDynSec.get())
            createRelDynSec(pLinker);

          m_pRelDynSec->reserveEntry(*m_pRelocFactory);
        }
      }
      break;
    case ELF::R_MIPS_REL32:
    case ELF::R_MIPS_26:
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
    case ELF::R_MIPS_PC16:
    case ELF::R_MIPS_SHIFT5:
    case ELF::R_MIPS_SHIFT6:
    case ELF::R_MIPS_64:
    case ELF::R_MIPS_GOT_PAGE:
    case ELF::R_MIPS_GOT_OFST:
    case ELF::R_MIPS_SUB:
    case ELF::R_MIPS_INSERT_A:
    case ELF::R_MIPS_INSERT_B:
    case ELF::R_MIPS_DELETE:
    case ELF::R_MIPS_HIGHER:
    case ELF::R_MIPS_HIGHEST:
    case ELF::R_MIPS_SCN_DISP:
    case ELF::R_MIPS_REL16:
    case ELF::R_MIPS_ADD_IMMEDIATE:
    case ELF::R_MIPS_PJUMP:
    case ELF::R_MIPS_RELGOT:
    case ELF::R_MIPS_JALR:
    case ELF::R_MIPS_GLOB_DAT:
    case ELF::R_MIPS_COPY:
    case ELF::R_MIPS_JUMP_SLOT:
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
      if (NULL == m_pGOT.get())
        createGOTSec(pLinker);

      m_pGOT->reserveEntry();
      break;
    case ELF::R_MIPS_GPREL32:
    case ELF::R_MIPS_GPREL16:
    case ELF::R_MIPS_LITERAL:
      break;
    case ELF::R_MIPS_GOT_DISP:
    case ELF::R_MIPS_GOT_HI16:
    case ELF::R_MIPS_CALL_HI16:
    case ELF::R_MIPS_GOT_LO16:
    case ELF::R_MIPS_CALL_LO16:
      break;
    case ELF::R_MIPS_TLS_DTPMOD32:
    case ELF::R_MIPS_TLS_DTPREL32:
    case ELF::R_MIPS_TLS_DTPMOD64:
    case ELF::R_MIPS_TLS_DTPREL64:
    case ELF::R_MIPS_TLS_GD:
    case ELF::R_MIPS_TLS_LDM:
    case ELF::R_MIPS_TLS_DTPREL_HI16:
    case ELF::R_MIPS_TLS_DTPREL_LO16:
    case ELF::R_MIPS_TLS_GOTTPREL:
    case ELF::R_MIPS_TLS_TPREL32:
    case ELF::R_MIPS_TLS_TPREL64:
    case ELF::R_MIPS_TLS_TPREL_HI16:
    case ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unknown relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine("for the local symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
  }
}

void MipsGNULDBackend::scanGlobalRelocation(Relocation& pReloc,
                                            MCLinker& pLinker,
                                            unsigned int pType)
{
  ResolveInfo* rsym = pReloc.symInfo();

  if (isSymbolNeedsPLT(*rsym, pType) /* TODO: check the sym hasn't a PLT offset */) {
    if (Output::DynObj == pType) {
      // TODO: (simon) Reserve .MIPS.stubs entry
    }
    else if (isSymbolNeedsDynRel(*rsym, pType)) {
      // TODO: (simon) Reserve .plt entry
    }
  }

  switch (pReloc.type()){
    case ELF::R_MIPS_NONE:
    case ELF::R_MIPS_INSERT_A:
    case ELF::R_MIPS_INSERT_B:
    case ELF::R_MIPS_DELETE:
    case ELF::R_MIPS_TLS_DTPMOD64:
    case ELF::R_MIPS_TLS_DTPREL64:
    case ELF::R_MIPS_REL16:
    case ELF::R_MIPS_ADD_IMMEDIATE:
    case ELF::R_MIPS_PJUMP:
    case ELF::R_MIPS_RELGOT:
    case ELF::R_MIPS_TLS_TPREL64:
      break;
    case ELF::R_MIPS_32:
    case ELF::R_MIPS_64:
    case ELF::R_MIPS_HI16:
    case ELF::R_MIPS_LO16:
      if (isSymbolNeedsDynRel(*rsym, pType)) {
        if (NULL == m_pRelDynSec.get())
          createRelDynSec(pLinker);

        m_pRelDynSec->reserveEntry(*m_pRelocFactory);
      }
      break;
    case ELF::R_MIPS_GOT16:
    case ELF::R_MIPS_CALL16:
    case ELF::R_MIPS_GOT_DISP:
    case ELF::R_MIPS_GOT_HI16:
    case ELF::R_MIPS_CALL_HI16:
    case ELF::R_MIPS_GOT_LO16:
    case ELF::R_MIPS_CALL_LO16:
    case ELF::R_MIPS_GOT_PAGE:
    case ELF::R_MIPS_GOT_OFST:
      if (NULL == m_pGOT.get())
        createGOTSec(pLinker);

      m_pGOT->reserveEntry();
      break;
    case ELF::R_MIPS_LITERAL:
    case ELF::R_MIPS_GPREL32:
      llvm::report_fatal_error(llvm::Twine("Relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine(" is not defined for the "
                                           "global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
      break;
    case ELF::R_MIPS_GPREL16:
      break;
    case ELF::R_MIPS_26:
    case ELF::R_MIPS_PC16:
      break;
    case ELF::R_MIPS_16:
    case ELF::R_MIPS_SHIFT5:
    case ELF::R_MIPS_SHIFT6:
    case ELF::R_MIPS_SUB:
    case ELF::R_MIPS_HIGHER:
    case ELF::R_MIPS_HIGHEST:
    case ELF::R_MIPS_SCN_DISP:
      break;
    case ELF::R_MIPS_TLS_DTPREL32:
    case ELF::R_MIPS_TLS_GD:
    case ELF::R_MIPS_TLS_LDM:
    case ELF::R_MIPS_TLS_DTPREL_HI16:
    case ELF::R_MIPS_TLS_DTPREL_LO16:
    case ELF::R_MIPS_TLS_GOTTPREL:
    case ELF::R_MIPS_TLS_TPREL32:
    case ELF::R_MIPS_TLS_TPREL_HI16:
    case ELF::R_MIPS_TLS_TPREL_LO16:
      break;
    case ELF::R_MIPS_REL32:
      break;
    case ELF::R_MIPS_JALR:
      break;
    case ELF::R_MIPS_COPY:
    case ELF::R_MIPS_GLOB_DAT:
    case ELF::R_MIPS_JUMP_SLOT:
      llvm::report_fatal_error(llvm::Twine("Relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine("for the global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("' should only be seen "
                                           "by the dynamic linker"));
      break;
    default:
      llvm::report_fatal_error(llvm::Twine("Unknown relocation ") +
                               llvm::Twine(pReloc.type()) +
                               llvm::Twine("for the global symbol `") +
                               pReloc.symInfo()->name() +
                               llvm::Twine("'."));
  }
}

void MipsGNULDBackend::createRelDynSec(MCLinker& pLinker)
{
  // For output relocation section, create output LDSection directly
  LDSection& sec = pLinker.getOrCreateOutputSectHdr(".rel.dyn",
                                         LDFileFormat::Relocation,
                                         ELF::SHT_REL,
                                         ELF::SHF_ALLOC);

  llvm::MCSectionData& data = pLinker.getOrCreateSectData(sec);

  unsigned int size = bitclass() / 8 * 2 ;
  m_pRelDynSec.reset(new MipsDynRelSection(sec, data, size));
}

void MipsGNULDBackend::createGOTSec(MCLinker& pLinker)
{
  // For target dependent ouput section, create ouput LDSection directly
  LDSection& sec = pLinker.getOrCreateOutputSectHdr(".got",
                                                    LDFileFormat::Target,
                                                    ELF::SHT_PROGBITS,
                                                    ELF::SHF_ALLOC | ELF::SHF_WRITE);

  llvm::MCSectionData& data = pLinker.getOrCreateSectData(sec);

  m_pGOT.reset(new MipsGOT(sec, data));
}

void MipsGNULDBackend::createMipsStubsSec(MCLinker& pLinker)
{
  LDSection& sec = pLinker.createSectHdr(".MIPS.stubs",
                                         LDFileFormat::Target,
                                         ELF::SHT_PROGBITS,
                                         ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);

  // TODO: (simon) Create wrapper
}

void MipsGNULDBackend::createPltSec(MCLinker& pLinker)
{
  if (NULL == m_pGOT.get())
    createGOTSec(pLinker);

  if (NULL == m_pRelDynSec.get())
    createRelDynSec(pLinker);

  LDSection& secGotPlt = pLinker.createSectHdr(".got.plt",
                                               LDFileFormat::Target,
                                               ELF::SHT_PROGBITS,
                                               ELF::SHF_ALLOC | ELF::SHF_WRITE);

  LDSection& secPlt = pLinker.createSectHdr(".plt",
                                            LDFileFormat::Target,
                                            ELF::SHT_PROGBITS,
                                            ELF::SHF_ALLOC | ELF::SHF_EXECINSTR);

  // TODO: (simon) Create wrapper
}

bool MipsGNULDBackend::isSymbolNeedsDynRel(ResolveInfo& pSym,
                                           unsigned int pType)
{
  if(pSym.isUndef() && (pType==Output::Exec))
    return false;
  if(pSym.isAbsolute())
    return false;
  if(pType==Output::DynObj)
    return true;
  if(pSym.isDyn() || pSym.isUndef())
    return true;

  return false;
}

bool MipsGNULDBackend::isSymbolNeedsPLT(ResolveInfo& pSym,
                                        unsigned int pType)
{
  // FIXME: (simon) extend conditions
  return Output::DynObj == pType &&
         ResolveInfo::Function == pSym.type() &&
         pSym.isDyn() || pSym.isUndef();
}

MipsGOT& MipsGNULDBackend::getGOT()
{
  assert(NULL != m_pGOT.get());
  return *m_pGOT.get();
}

const MipsGOT& MipsGNULDBackend::getGOT() const
{
  assert(NULL != m_pGOT.get());
  return *m_pGOT.get();
}

namespace mcld {

//===----------------------------------------------------------------------===//
/// createMipsLDBackend - the help funtion to create corresponding MipsLDBackend
///
static TargetLDBackend* createMipsLDBackend(const llvm::Target& pTarget,
                                            const std::string& pTriple)
{
  Triple theTriple(pTriple);
  if (theTriple.isOSDarwin()) {
    assert(0 && "MachO linker is not supported yet");
  }
  if (theTriple.isOSWindows()) {
    assert(0 && "COFF linker is not supported yet");
  }
  return new MipsGNULDBackend();
}

} // namespace of mcld

//=============================
// Force static initialization.
extern "C" void LLVMInitializeMipsLDBackend() {
  // Register the linker backend
  mcld::TargetRegistry::RegisterTargetLDBackend(TheMipselTarget,
                                                createMipsLDBackend);
}
