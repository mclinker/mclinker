/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <Jush.Lu@mediatek.com>                                          *
 *   Luba Tang <luba.tang@mediatek.com>                                      *
 ****************************************************************************/
#ifndef MCLD_CONTEXT_H
#define MCLD_CONTEXT_H
#ifdef ENABLE_UNITTEST
#include <gtest.h>
#endif

#include <llvm/ADT/DenseMap.h>
#include <llvm/ADT/ilist.h>
#include <llvm/ADT/OwningPtr.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCSectionELF.h>
#include <llvm/MC/MCSymbol.h>
#include <llvm/Support/Allocator.h>
#include <llvm/Support/MemoryBuffer.h>
#include <mcld/ADT/StringMap.h>
#include <mcld/LD/Relocation.h>
#include <mcld/Support/FileSystem.h>
#include <map>
#include <utility>


using namespace llvm;

typedef llvm::StringMap<const MCSectionELF*> ELFUniqueMapTy;

namespace llvm
{
  class MCSection;
  class MCSectionData;
} // namespace of llvm

namespace mcld
{

/** \class MCLDContext
 *  \brief MCLDContext stores the data which a object file should has
 */
class MCLDContext
{
private:
  /// Allocator - Allocator object used for creating machine code objects.
  ///
  /// We use a bump pointer allocator to avoid the need to track all allocated
  /// objects.
  BumpPtrAllocator Allocator;

public:
  MCLDContext() : Allocator(), SymTab(Allocator) {
    ELFUniquingMap = 0;
  }

  ~MCLDContext() {}

  typedef iplist<MCSectionData> SectionDataListType;
  typedef iplist<MCSymbolData> SymbolDataListType;

  typedef llvm::StringMap<MCSymbol*, BumpPtrAllocator&> SymbolTableType;

  typedef SectionDataListType::const_iterator const_iterator;
  typedef SectionDataListType::iterator iterator;

  typedef SymbolDataListType::const_iterator const_symbol_iterator;
  typedef SymbolDataListType::iterator symbol_iterator;

  typedef std::vector<RelocationEntry>::const_iterator const_reloc_iterator;
  typedef std::vector<RelocationEntry>::iterator reloc_iterator;

  //Section List Access
  SectionDataListType Sections;
  SymbolDataListType Symbols;
  SymbolTableType SymTab;
  RelocationInfo m_Reloc;

  const SectionDataListType &getSectionList() const { return Sections; }
  SectionDataListType &getSectionList() { return Sections; }

  iterator begin() { return Sections.begin(); }
  const_iterator begin() const { return Sections.begin(); }

  iterator end() { return Sections.end(); }
  const_iterator end() const { return Sections.end(); }

  size_t size() const { return Sections.size(); }

  //Symbol List Access
  const SymbolDataListType &getSymbolList() const { return Symbols; }
  SymbolDataListType &getSymbolList() { return Symbols; }

  symbol_iterator symbol_begin() { return Symbols.begin(); }
  const_symbol_iterator symbol_begin() const { return Symbols.begin(); }

  symbol_iterator symbol_end() { return Symbols.end(); }
  const_symbol_iterator symbol_end() const { return Symbols.end(); }

  // Relocation sequential access
  reloc_iterator reloc_begin() { return m_Reloc.entries.begin(); }
  const_reloc_iterator reloc_begin() const { return m_Reloc.entries.begin(); }
  reloc_iterator reloc_end() { return m_Reloc.entries.end(); }
  const_reloc_iterator reloc_end() const { return m_Reloc.entries.end(); }


  // FIXME: Avoid this indirection?
  DenseMap<const MCSection*, MCSectionData*> SectionMap;

  // FIXME: Avoid this indirection?
  DenseMap<const MCSymbol*,MCSymbolData*> SymbolMap;

  void *ELFUniquingMap;

public:
  MCSymbol *getOrCreateSymbol(StringRef Name);

  const MCSectionELF *getELFSection(StringRef SectionName, unsigned Type,
                                    unsigned Flags, SectionKind Kind);

  const MCSectionELF *getELFSection(StringRef SectionName, unsigned Type,
                                    unsigned Flags, SectionKind Kind,
                                    unsigned EntrySize, StringRef Group);

  MCSectionData &getOrCreateSectionData(const MCSection &Section,
                                        bool *Created = 0);

  MCSymbolData &getSymbolData(const MCSymbol &Symbol) const;

  MCSymbolData &getOrCreateSymbolData(const MCSymbol &Symbol,
                                      bool *Create = 0);

  RelocationInfo& getRelocInfo() { return m_Reloc; }
  const RelocationInfo& getRelocInfo() const { return m_Reloc; }

  void dump();

  void *Allocate(unsigned Size, unsigned Align = 8) {
    return Allocator.Allocate(Size, Align);
  }

  void Deallocate(void *Ptr) {
  }
};


} // namespace of mcld

// operator new and delete aren't allowed inside namespaces.
// The throw specifications are mandated by the standard.
/// @brief Placement new for using the MCContext's allocator.
///
/// This placement form of operator new uses the MCContext's allocator for
/// obtaining memory. It is a non-throwing new, which means that it returns
/// null on error. (If that is what the allocator does. The current does, so if
/// this ever changes, this operator will have to be changed, too.)
/// Usage looks like this (assuming there's an MCContext 'Context' in scope):
/// @code
/// // Default alignment (16)
/// IntegerLiteral *Ex = new (Context) IntegerLiteral(arguments);
/// // Specific alignment
/// IntegerLiteral *Ex2 = new (Context, 8) IntegerLiteral(arguments);
/// @endcode
/// Please note that you cannot use delete on the pointer; it must be
/// deallocated using an explicit destructor call followed by
/// @c Context.Deallocate(Ptr).
///
/// @param Bytes The number of bytes to allocate. Calculated by the compiler.
/// @param C The MCContext that provides the allocator.
/// @param Alignment The alignment of the allocated memory (if the underlying
///                  allocator supports it).
/// @return The allocated memory. Could be NULL.
inline void *operator new(size_t Bytes, mcld::MCLDContext &C,
                                size_t Alignment = 16) throw () {
  return C.Allocate(Bytes, Alignment);
}

/// @brief Placement delete companion to the new above.
///
/// This operator is just a companion to the new above. There is no way of
/// invoking it directly; see the new operator for more details. This operator
/// is called implicitly by the compiler if a placement new expression using
/// the MCContext throws in the object constructor.
inline void operator delete(void *Ptr, mcld::MCLDContext &C, size_t)
              throw () {
  C.Deallocate(Ptr);
}

#endif


