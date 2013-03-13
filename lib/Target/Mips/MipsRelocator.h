//===- MipsRelocator.h --------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MIPS_RELOCATION_FACTORY_H
#define MIPS_RELOCATION_FACTORY_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/DenseMapInfo.h>
#include <mcld/LD/Relocator.h>
#include <mcld/Support/GCFactory.h>
#include "MipsLDBackend.h"

namespace mcld {

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
                      LDSection& pSection);

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

  Result applyRelocation(Relocation& pRelocation);

  const Input& getApplyingInput() const
  { return *m_pApplyingInput; }

  MipsGNULDBackend& getTarget()
  { return m_Target; }

  const MipsGNULDBackend& getTarget() const
  { return m_Target; }

  // Get last calculated AHL.
  int32_t getAHL() const
  { return m_AHL; }

  // Set last calculated AHL.
  void setAHL(int32_t pAHL)
  { m_AHL = pAHL; }

  /// getPLTOffset - initialize PLT-related entries for the symbol
  /// @return - return address of PLT entry
  uint64_t getPLTAddress(ResolveInfo& rsym);

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

private:
  typedef std::pair<Fragment*, Fragment*> PLTDescriptor;
  typedef llvm::DenseMap<const ResolveInfo*, PLTDescriptor> SymPLTMap;

private:
  MipsGNULDBackend& m_Target;
  SymPLTMap m_SymPLTMap;
  Input* m_pApplyingInput;
  int32_t m_AHL;

private:
  void scanLocalReloc(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      const LDSection& pSection);

  void scanGlobalReloc(Relocation& pReloc,
                       IRBuilder& pBuilder,
                       const LDSection& pSection);

  /// addCopyReloc - add a copy relocation into .rel.dyn for pSym
  /// @param pSym - A resolved copy symbol that defined in BSS section
  void addCopyReloc(ResolveInfo& pSym);

  /// defineSymbolforCopyReloc - allocate a space in BSS section and
  /// and force define the copy of pSym to BSS section
  /// @return the output LDSymbol of the copy symbol
  LDSymbol& defineSymbolforCopyReloc(IRBuilder& pBuilder,
                                     const ResolveInfo& pSym);
};

} // namespace of mcld

#endif
