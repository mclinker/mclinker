//===- DebugString.h ------------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#ifndef MCLD_LD_DEBUGSTRING_H
#define MCLD_LD_DEBUGSTRING_H

#include <mcld/LD/MergedStringTable.h>

#include <vector>

namespace mcld {

class LDSection;
class Relocation;
class TargetLDBackend;

/** \class DebugString
 *  \brief DebugString represents the debug section .debug_str
 */
class DebugString
{
public:
  DebugString()
   : m_pSection(NULL), m_Success(true)
  {}

  /// processRelocs - process the relocation section. Get the strings used by
  /// the relcoations and add them into the merged string table.
  void processRelocs(LDSection& pSection, const TargetLDBackend& pBackend);

  /// sizeStringTable - set up the section size and determine the output offset
  /// of each strings.
  uint64_t sizeStringTable();

  /// applyOffset - apply the relocations those refer to debug string. This
  /// should be called after sizeStringTable.
  void applyOffset(TargetLDBackend& pBackend);

  /// emit - emit the section .debug_str
  void emit(MemoryRegion& pRegion);

  void setOutputSection(LDSection& pSection)
  { m_pSection = &pSection; }

  /// ---- observers ----- ///
  const LDSection* getSection() const { return m_pSection; }
  LDSection*       getSection()       { return m_pSection; }

  bool empty() const
  { return m_RelocStringList.empty(); }

  /// isSuccess - the mergeable debug strings are successfully proccessed or not
  bool isSuccess() const
  { return m_Success; }

private:
  typedef std::pair<Relocation&, MergedStringTable::StringMapEntryTy&> EntryTy;
  typedef std::vector<EntryTy> EntryListTy;
  typedef EntryListTy::iterator entry_iterator;
  typedef EntryListTy::const_iterator const_entry_iterator;

private:
  /// m_Section - the output LDSection of this .debug_str
  LDSection* m_pSection;

  EntryListTy m_RelocStringList;

  MergedStringTable m_StringTable;

  /// m_Success - the mergeable debug strings are successfully proccessed or not
  bool m_Success;
};

} // namespace of mcld

#endif

