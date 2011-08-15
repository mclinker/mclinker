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
#include <llvm/ADT/StringMap.h>
#include <llvm/MC/MCAssembler.h>
#include <llvm/MC/MCContext.h>
#include <llvm/MC/MCSectionELF.h>
#include <llvm/MC/MCSymbol.h>
#include <llvm/Support/Allocator.h>
#include <llvm/Support/MemoryBuffer.h>
#include <mcld/Support/FileSystem.h>
#include <map>
#include <utility>


using namespace llvm;

typedef StringMap<const MCSectionELF*> ELFUniqueMapTy;

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

  ~MCLDContext(){};

  typedef iplist<MCSectionData> SectionDataListType;
  typedef iplist<MCSymbolData> SymbolDataListType;

  typedef StringMap<MCSymbol*, BumpPtrAllocator&> SymbolTableType;

  typedef SectionDataListType::const_iterator const_iterator;
  typedef SectionDataListType::iterator iterator;

  typedef SymbolDataListType::const_iterator const_symbol_iterator;
  typedef SymbolDataListType::iterator symbol_iterator;


  //Section List Access
  SectionDataListType Sections;
  SymbolDataListType Symbols;
  SymbolTableType SymTab;

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


    // FIXME: Avoid this indirection?
    DenseMap<const MCSection*, MCSectionData*> SectionMap;

    // FIXME: Avoid this indirection?
    DenseMap<const MCSymbol*,MCSymbolData*> SymbolMap;

    void *ELFUniquingMap;

  public:
    MCSymbol *getOrCreateSymbol(StringRef Name);
    MCSymbol *createSymbol(StringRef Name);

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

  void dump();
};


} // namespace of mcld

#endif


