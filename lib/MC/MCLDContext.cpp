/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu    <Jush.Lu@mediatek.com>                                       *
 ****************************************************************************/
#include <mcld/MC/MCLDContext.h>

using namespace mcld;

MCSymbol *MCLDContext::getOrCreateSymbol(StringRef Name) {

  assert(!Name.empty() && "Normal symbols can not be unnamed!");

  //Do the lookup and get the entire StringMapEntry. We want access to the
  //key if we are creating the entry
  StringMapEntry<MCSymbol*> &Entry = SymTab.GetOrCreateValue(Name);
  MCSymbol *Sym = Entry.getValue();

  if (Sym)
    return Sym;

  Sym = createSymbol(Name);
  Entry.setValue(Sym);
  return Sym;
}

MCSymbol *MCLDContext::createSymbol(StringRef Name) {
  //Determine whether this is an assembler temporary or normal label, if used.
  bool isTemporary = false;

  //FIXME : use more implement from the same function in MCContext
  //StringMapEntry<bool> *NameEntry = &Used

  MCSymbol *Result = new(*this) MCSymbol(Name, isTemporary);
  return Result;
}

const MCSectionELF *MCLDContext::
getELFSection(StringRef SectionName, unsigned Type,
              unsigned Flags, SectionKind Kind) {
  return getELFSection(SectionName, Type, Flags, Kind, 0, "");
}

const MCSectionELF *MCLDContext::
getELFSection(StringRef SectionName, unsigned Type,
              unsigned Flags, SectionKind Kind,
              unsigned EntrySize, StringRef Group) {
  if (ELFUniquingMap == 0)
    ELFUniquingMap = new ELFUniqueMapTy(); 

  ELFUniqueMapTy &Map= *(ELFUniqueMapTy*)ELFUniquingMap;

  //Do the lookup, if we have a hit, return it.
  StringMapEntry<const MCSectionELF*> 
    &Entry = Map.GetOrCreateValue(SectionName);

  if (Entry.getValue()) return Entry.getValue();

  // Possibly refine the entry size first.
  if (!EntrySize) 
    EntrySize = MCSectionELF::DetermineEntrySize(Kind);

  MCSymbol *GroupSym = NULL;

  if (!Group.empty())
    GroupSym = getOrCreateSymbol(Group);

  MCSectionELF *Result = new(*this) MCSectionELF(Entry.getKey(), Type, Flags,
                                           Kind, EntrySize, GroupSym);

  Entry.setValue(Result);
  return Result;  
}


MCSectionData &MCLDContext::getOrCreateSectionData(const MCSection &Section,
                                                   bool *Created) {
  MCSectionData *&Entry = SectionMap[&Section];

  if (Created) *Created = !Entry;
  if (!Entry)
    Entry = new MCSectionData(Section, Sections);

  return *Entry;
}

MCSymbolData &MCLDContext::getSymbolData(const MCSymbol &Symbol) const {
  MCSymbolData *Entry = SymbolMap.lookup(&Symbol);
  assert(Entry && "Missing symbol data!");
  return *Entry;
}

MCSymbolData &MCLDContext::getOrCreateSymbolData(const MCSymbol &Symbol,
                                                 bool *Created) {
  MCSymbolData *&Entry = SymbolMap[&Symbol];

  if (Created) *Created = !Entry;
  if (!Entry)
    Entry = new MCSymbolData(Symbol, 0, 0, Symbols);
}

void MCLDContext::dump() {
  raw_ostream &OS = llvm::errs();

  OS << "MCELFObjectReader: \n";
  OS << "Sections:[";

  for (iterator it = begin(), ie = end(); it != ie; ++it) {
    if (it != begin()) OS << ",\n    ";
    it->dump();
  }
  OS << "  ] \n";
  OS << "    \n";

  OS << "Symbols:[";
  for (symbol_iterator it = symbol_begin(),
       ie = symbol_end(); it != ie; ++it) {
    OS<<"\n      ";
    it->dump();
  }
  OS << "  ] \n";
}


