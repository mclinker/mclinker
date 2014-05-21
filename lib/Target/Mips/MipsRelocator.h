//===- MipsRelocator.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_MIPS_MIPSRELOCATOR_H
#define TARGET_MIPS_MIPSRELOCATOR_H

#include <llvm/ADT/DenseMapInfo.h>
#include <mcld/LD/Relocator.h>
#include <mcld/Support/GCFactory.h>
#include "MipsLDBackend.h"

namespace mcld {

class MipsRelocationInfo;

/** \class MipsRelocator
 *  \brief MipsRelocator creates and destroys the Mips relocations.
 */
class MipsRelocator : public Relocator
{
public:
  enum ReservedEntryType {
    None          = 0,  // no reserved entry
    ReserveRel    = 1,  // reserve a dynamic relocation entry
    ReserveGot    = 2,  // reserve a GOT entry
    ReservePLT    = 4   // reserve a PLT entry
  };

public:
  MipsRelocator(MipsGNULDBackend& pParent, const LinkerConfig& pConfig);

  /// scanRelocation - determine the empty entries are needed or not and
  /// create the empty entries if needed.
  /// For Mips, the GOT, GP, and dynamic relocation entries are check to create.
  void scanRelocation(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      Module& pModule,
                      LDSection& pSection,
                      Input& pInput);

  /// initializeScan - do initialization before scan relocations in pInput
  /// @return - return true for initialization success
  bool initializeScan(Input& pInput);

  /// finalizeScan - do finalization after scan relocations in pInput
  /// @return - return true for finalization success
  bool finalizeScan(Input& pInput);

  /// initializeApply - do initialization before apply relocations in pInput
  /// @return - return true for initialization success
  bool initializeApply(Input& pInput);

  /// finalizeApply - do finalization after apply relocations in pInput
  /// @return - return true for finalization success
  bool finalizeApply(Input& pInput);

  Result applyRelocation(Relocation& pReloc);

  const Input& getApplyingInput() const
  { return *m_pApplyingInput; }

  MipsGNULDBackend& getTarget()
  { return m_Target; }

  const MipsGNULDBackend& getTarget() const
  { return m_Target; }

  /// postponeRelocation - save R_MIPS_LO16 paired relocations
  /// like R_MISP_HI16 and R_MIPS_GOT16 for a future processing.
  void postponeRelocation(Relocation& pReloc);

  /// applyPostponedRelocations - apply all postponed relocations
  /// paired with the R_MIPS_LO16 one.
  void applyPostponedRelocations(MipsRelocationInfo& pLo16Reloc);

  /// isGpDisp - return true if relocation is against _gp_disp symbol.
  bool isGpDisp(const Relocation& pReloc) const;

  /// getGPAddress - return address of _gp symbol.
  Address getGPAddress();

  /// getGP0 - the gp value used to create the relocatable objects
  /// in the processing input.
  Address getGP0();

  /// getLocalGOTEntry - initialize and return a local GOT entry
  /// for this relocation.
  Fragment& getLocalGOTEntry(MipsRelocationInfo& pReloc,
                             Relocation::DWord entryValue);

  /// getGlobalGOTEntry - initialize and return a global GOT entry
  /// for this relocation.
  Fragment& getGlobalGOTEntry(MipsRelocationInfo& pReloc);

  /// getGOTOffset - return offset of corresponded GOT entry.
  Address getGOTOffset(MipsRelocationInfo& pReloc);

  /// createDynRel - initialize dynamic relocation for the relocation.
  void createDynRel(MipsRelocationInfo& pReloc);

  /// getPLTOffset - initialize PLT-related entries for the symbol
  /// @return - return address of PLT entry
  uint64_t getPLTAddress(ResolveInfo& rsym);

  /// calcAHL - calculate combined addend used
  /// by R_MIPS_HI16 and R_MIPS_GOT16 relocations.
  uint64_t calcAHL(const MipsRelocationInfo& pHiReloc);

  /// isN64ABI - check current ABI
  bool isN64ABI() const;

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

protected:
  /// setupRelDynEntry - create dynamic relocation entry.
  virtual void setupRelDynEntry(FragmentRef& pFragRef, ResolveInfo* pSym) = 0;

  /// isLocalReloc - handle relocation as a local symbol
  bool isLocalReloc(ResolveInfo& pSym) const;

private:
  typedef std::pair<Fragment*, Fragment*> PLTDescriptor;
  typedef llvm::DenseMap<const ResolveInfo*, PLTDescriptor> SymPLTMap;
  typedef llvm::DenseSet<Relocation*> RelocationSet;
  typedef llvm::DenseMap<const ResolveInfo*, RelocationSet> SymRelocSetMap;

private:
  MipsGNULDBackend& m_Target;
  SymPLTMap m_SymPLTMap;
  Input* m_pApplyingInput;
  SymRelocSetMap m_PostponedRelocs;
  MipsRelocationInfo* m_CurrentLo16Reloc;

private:
  void scanLocalReloc(MipsRelocationInfo& pReloc,
                      IRBuilder& pBuilder,
                      const LDSection& pSection);

  void scanGlobalReloc(MipsRelocationInfo& pReloc,
                       IRBuilder& pBuilder,
                       const LDSection& pSection);

  /// isPostponed - relocation applying needs to be postponed.
  bool isPostponed(const Relocation& pReloc) const;

  /// addCopyReloc - add a copy relocation into .rel.dyn for pSym
  /// @param pSym - A resolved copy symbol that defined in BSS section
  void addCopyReloc(ResolveInfo& pSym);

  /// defineSymbolforCopyReloc - allocate a space in BSS section and
  /// and force define the copy of pSym to BSS section
  /// @return the output LDSymbol of the copy symbol
  LDSymbol& defineSymbolforCopyReloc(IRBuilder& pBuilder,
                                     const ResolveInfo& pSym);

  /// isRel - returns true if REL relocation record format is expected
  bool isRel() const;
};

/** \class Mips32Relocator
 *  \brief Mips32Relocator creates and destroys the Mips 32-bit relocations.
 */
class Mips32Relocator : public MipsRelocator
{
public:
  Mips32Relocator(Mips32GNULDBackend& pParent, const LinkerConfig& pConfig);

private:
  // MipsRelocator
  void setupRelDynEntry(FragmentRef& pFragRef, ResolveInfo* pSym);
};

/** \class Mips64Relocator
 *  \brief Mips64Relocator creates and destroys the Mips 64-bit relocations.
 */
class Mips64Relocator : public MipsRelocator
{
public:
  Mips64Relocator(Mips64GNULDBackend& pParent, const LinkerConfig& pConfig);

private:
  // MipsRelocator
  void setupRelDynEntry(FragmentRef& pFragRef, ResolveInfo* pSym);
};

} // namespace of mcld

#endif
