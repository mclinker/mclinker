//===- ELFEXEWriter.cpp ---------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "mcld/LD/ELFEXEWriter.h"

using namespace mcld;

void ELFEXEWriter::writeHeader(){
  RegionWriter Writer(
    m_pArea->request(0, sizeof(llvm::ELF::Elf32_Ehdr)), false);

  Writer.Write8(0x7f); // e_ident[EI_MAG0]
  Writer.Write8('E');  // e_ident[EI_MAG1]
  Writer.Write8('L');  // e_ident[EI_MAG2]
  Writer.Write8('F');  // e_ident[EI_MAG3]
}


void ELFEXEWriter::writeFile() {

  writeHeader();

}

