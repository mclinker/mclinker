/*****************************************************************************
 *   The MCLinker Project, Copyright (C), 2011 -                             *
 *   Embedded and Web Computing Lab, National Taiwan University              *
 *   MediaTek, Inc.                                                          *
 *                                                                           *
 *   Jush Lu <jush.lu@mediatek.com> (owner)                                  *
 ****************************************************************************/

#include <mcld/LD/ELFDSOWriter.h>

using namespace mcld;

void ELFDSOWriter::writeHeader(){
  RegionWriter Writer(
    m_pArea->request(0, sizeof(llvm::ELF::Elf32_Ehdr)), false);

  Writer.Write8(0x7f); // e_ident[EI_MAG0]
  Writer.Write8('E');  // e_ident[EI_MAG1]
  Writer.Write8('L');  // e_ident[EI_MAG2]
  Writer.Write8('F');  // e_ident[EI_MAG3]
}


void ELFDSOWriter::writeFile() {

  writeHeader();

}
