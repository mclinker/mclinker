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

#ifndef ELF_RELOC_HXX
#define ELF_RELOC_HXX

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFReloc<Bitwidth> *
ELFReloc_CRTP<Bitwidth>::readRela(Archiver &AR, size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ELFRelocTy> sh(new ELFRelocTy());

  if (!sh->serializeRela(AR)) {
    // Unable to read the structure.  Return NULL.
    return 0;
  }

  if (!sh->isValid()) {
    // Rel read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  return sh.take();
}

template <unsigned Bitwidth>
template <typename Archiver>
inline ELFReloc<Bitwidth> *
ELFReloc_CRTP<Bitwidth>::readRel(Archiver &AR, size_t index) {
  if (!AR) {
    // Archiver is in bad state before calling read function.
    // Return NULL and do nothing.
    return 0;
  }

  llvm::OwningPtr<ELFRelocTy> sh(new ELFRelocTy());

  sh->r_addend = 0;
  if (!sh->serializeRel(AR)) {
    return 0;
  }

  if (!sh->isValid()) {
    // Rel read from archiver is not valid.  Return NULL.
    return 0;
  }

  // Set the section header index
  sh->index = index;

  return sh.take();
}

#endif // ELF_RELOC_HXX
