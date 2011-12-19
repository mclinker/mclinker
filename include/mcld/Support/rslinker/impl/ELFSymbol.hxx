/*
 * Copyright 2011, The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ELF_SYMBOL_HXX
#define ELF_SYMBOL_HXX

#include "mcld/Support/rslinker/ELFSectionHeaderTable.h"
#include "mcld/Support/rslinker/ELFSection.h"
#include "mcld/Support/rslinker/ELFSectionStrTab.h"

#include "mcld/Support/rslinker/ELFObject.h"
#include "mcld/Support/rslinker/ELFSectionHeaderTable.h"

#include "mcld/Support/rslinker/ELF.h"

template <unsigned Bitwidth>
inline char const *ELFSymbol_CRTP<Bitwidth>::getName(bool isDynsym) const {
  std::string symtab( isDynsym ? ".dynstr" : ".strtab");
  ELFSectionHeaderTableTy const &shtab = *owner->getSectionHeaderTable();
  size_t const index = shtab.getByName(symtab)->getIndex();
  ELFSectionTy const *section = owner->getSectionByIndex(index);
  ELFSectionStrTabTy const &strtab =
    *static_cast<ELFSectionStrTabTy const *>(section);
  return strtab[getNameIndex()];
}

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFSymbol<Bitwidth> *
ELFSymbol_CRTP<Bitwidth>::read(Archiver &AR,
                               ELFObjectTy const *owner,
                               size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ELFSymbolTy> sh(new ELFSymbolTy());

  if (!sh->serialize(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!sh->isValid()) {
    // SymTabEntry read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  // Set the owner elf object
  sh->owner = owner;

  return sh.take();
}

#endif // ELF_SYMBOL_HXX
