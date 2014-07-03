//===-  ARMRelocator.h ----------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef TARGET_ARM_ARMRELOCATOR_H
#define TARGET_ARM_ARMRELOCATOR_H

#include <mcld/LD/Relocator.h>
#include <mcld/Target/GOT.h>
#include <mcld/Target/KeyEntryMap.h>
#include "ARMLDBackend.h"

namespace mcld {

/** \class ARMRelocator
 *  \brief ARMRelocator creates and destroys the ARM relocations.
 *
 */
class ARMRelocator : public Relocator
{
public:
  typedef KeyEntryMap<ResolveInfo, ARMGOTEntry> SymGOTMap;
  typedef KeyEntryMap<ResolveInfo, ARMPLT1> SymPLTMap;

  /** \enum ReservedEntryType
   *  \brief The reserved entry type of reserved space in ResolveInfo.
   *
   *  This is used for sacnRelocation to record what kinds of entries are
   *  reserved for this resolved symbol In ARM, there are three kinds of
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
  ARMRelocator(ARMGNULDBackend& pParent, const LinkerConfig& pConfig);
  ~ARMRelocator();

  Result applyRelocation(Relocation& pRelocation);

  ARMGNULDBackend& getTarget()
  { return m_Target; }

  const ARMGNULDBackend& getTarget() const
  { return m_Target; }

  const char* getName(Relocation::Type pType) const;

  Size getSize(Relocation::Type pType) const;

  const SymGOTMap& getSymGOTMap() const { return m_SymGOTMap; }
  SymGOTMap&       getSymGOTMap()       { return m_SymGOTMap; }

  const SymPLTMap& getSymPLTMap() const { return m_SymPLTMap; }
  SymPLTMap&       getSymPLTMap()       { return m_SymPLTMap; }

  const SymGOTMap& getSymGOTPLTMap() const { return m_SymGOTPLTMap; }
  SymGOTMap&       getSymGOTPLTMap()       { return m_SymGOTPLTMap; }

  /// scanRelocation - determine the empty entries are needed or not and create
  /// the empty entries if needed.
  /// For ARM, following entries are check to create:
  /// - GOT entry (for .got section)
  /// - PLT entry (for .plt section)
  /// - dynamin relocation entries (for .rel.plt and .rel.dyn sections)
  void scanRelocation(Relocation& pReloc,
                      IRBuilder& pBuilder,
                      Module& pModule,
                      LDSection& pSection,
                      Input& pInput);


  /// mayHaveFunctionPointerAccess - check if the given reloc would possibly
  /// access a function pointer.
  virtual bool mayHaveFunctionPointerAccess(const Relocation& pReloc) const;

private:
  void scanLocalReloc(Relocation& pReloc, const LDSection& pSection);

  void scanGlobalReloc(Relocation& pReloc,
                       IRBuilder& pBuilder,
                       const LDSection& pSection);

  void checkValidReloc(Relocation& pReloc) const;

  /// addCopyReloc - add a copy relocation into .rel.dyn for pSym
  /// @param pSym - A resolved copy symbol that defined in BSS section
  void addCopyReloc(ResolveInfo& pSym);

  /// defineSymbolforCopyReloc - allocate a space in BSS section and
  /// and force define the copy of pSym to BSS section
  /// @return the output LDSymbol of the copy symbol
  LDSymbol& defineSymbolforCopyReloc(IRBuilder& pLinker,
                                     const ResolveInfo& pSym);

private:
  ARMGNULDBackend& m_Target;
  SymGOTMap m_SymGOTMap;
  SymPLTMap m_SymPLTMap;
  SymGOTMap m_SymGOTPLTMap;
};

} // namespace of mcld

#endif

