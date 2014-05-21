//===-  AArch64Relocator.h ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_AARCH64_AARCH64RELOCATOR_H
#define TARGET_AARCH64_AARCH64RELOCATOR_H

#include <mcld/LD/Relocator.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/KeyEntryMap.h>
#include "AArch64LDBackend.h"

namespace mcld {
// FIXME: llvm::ELF doesn't define AArch64 dynamic relocation types
enum {
  // static relocations
  R_AARCH64_ADR_PREL_PG_HI21_NC = 0x114,
  // dyanmic rlocations
  R_AARCH64_COPY                = 1024,
  R_AARCH64_GLOB_DAT            = 1025,
  R_AARCH64_JUMP_SLOT           = 1026,
  R_AARCH64_RELATIVE            = 1027,
  R_AARCH64_TLS_DTPREL64        = 1028,
  R_AARCH64_TLS_DTPMOD64        = 1029,
  R_AARCH64_TLS_TPREL64         = 1030,
  R_AARCH64_TLSDESC             = 1031,
  R_AARCH64_IRELATIVE           = 1032
};

/** \class AArch64Relocator
 *  \brief AArch64Relocator creates and destroys the AArch64 relocations.
 *
 */
class AArch64Relocator : public Relocator
{
public:
  typedef KeyEntryMap<ResolveInfo, AArch64GOTEntry> SymGOTMap;
  typedef KeyEntryMap<ResolveInfo, AArch64PLT1> SymPLTMap;
  typedef KeyEntryMap<Relocation, Relocation> RelRelMap;

  /** \enum ReservedEntryType
   *  \brief The reserved entry type of reserved space in ResolveInfo.
   *
   *  This is used for sacnRelocation to record what kinds of entries are
   *  reserved for this resolved symbol In AArch64, there are three kinds of
   *  entries, GOT, PLT, and dynamic reloction.
   *
   *  bit:  3     2     1     0
   *   |    | PLT | GOT | Rel |
   *
   *  value    Name         - Description
   *
   *  0000     None         - no reserved entry
   *  0001     ReserveRel   - reserve an dynamic relocation entry
   *  0010     ReserveGOT   - reserve an GOT entry
   *  0100     ReservePLT   - reserve an PLT entry and the corresponding GOT,
   *
   */
  enum ReservedEntryType {
    None         = 0,
    ReserveRel   = 1,
    ReserveGOT   = 2,
    ReservePLT   = 4,
  };

  /** \enum EntryValue
   *  \brief The value of the entries. The symbol value will be decided at after
   *  layout, so we mark the entry during scanRelocation and fill up the actual
   *  value when applying relocations.
   */
  enum EntryValue {
    Default = 0,
    SymVal  = 1
  };

public:
  AArch64Relocator(AArch64GNULDBackend& pParent, const LinkerConfig& pConfig);
  ~AArch64Relocator();

  Result applyRelocation(Relocation& pRelocation);

  AArch64GNULDBackend& getTarget()
  { return m_Target; }

  const AArch64GNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

  const SymPLTMap& getSymPLTMap() const { return m_SymPLTMap; }
  SymPLTMap&       getSymPLTMap()       { return m_SymPLTMap; }

  const SymGOTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

  const RelRelMap& getRelRelMap() const { return m_RelRelMap; }
  RelRelMap&       getRelRelMap()       { return m_RelRelMap; }

  /// scanRelocation - determine the empty entries are needed or not and create
  /// the empty entries if needed.
  /// For AArch64, following entries are check to create:
  /// - GOT entry (for .got section)
  /// - PLT entry (for .plt section)
  /// - dynamin relocation entries (for .rel.plt and .rel.dyn sections)
  void scanRelocation(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      Module& pModule,
                      LDSection& pSection,
                      Input& pInput);

private:
  void scanLocalReloc(Relocation& pReloc, const LDSection& pSection);

  void scanGlobalReloc(Relocation& pReloc,
                       IRBuilder& pBuilder,
                       const LDSection& pSection);

  /// addCopyReloc - add a copy relocation into .rel.dyn for pSym
  /// @param pSym - A resolved copy symbol that defined in BSS section
  void addCopyReloc(ResolveInfo& pSym);

  /// defineSymbolforCopyReloc - allocate a space in BSS section and
  /// and force define the copy of pSym to BSS section
  /// @return the output LDSymbol of the copy symbol
  LDSymbol& defineSymbolforCopyReloc(IRBuilder& pLinker,
                                     const ResolveInfo& pSym);

private:
  AArch64GNULDBackend& m_Target;
  SymGOTMap m_SymGOTMap;
  SymPLTMap m_SymPLTMap;
  SymGOTMap m_SymGOTPLTMap;
  RelRelMap m_RelRelMap;
};

} // namespace of mcld

#endif

