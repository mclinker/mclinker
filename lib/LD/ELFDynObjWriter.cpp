//===- ELFDynObjWriter.cpp ------------------------------------------------===//
//
//                     The MCLinker Project
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
#include "mcld/LD/ELFDynObjWriter.h"
#include "mcld/Target/TargetLDBackend.h"
#include "mcld/MC/MCLDInput.h"
#include "mcld/MC/MCLDOutput.h"
#include "mcld/MC/MCLDInfo.h"
#include "mcld/MC/MCLinker.h"
#include "mcld/Support/ScopedWriter.h"
#include "llvm/Support/ELF.h"

#include <iostream>

using namespace mcld;
using namespace std;

//==========================
// ELFDynObjWriter
ELFDynObjWriter::ELFDynObjWriter(TargetLDBackend& pBackend, MCLinker& pLinker)
  : DynObjWriter(pBackend), ELFWriter(), m_Linker(pLinker) {

}

ELFDynObjWriter::~ELFDynObjWriter()
{
}

bool ELFDynObjWriter::WriteObject()
{
  if (!WriteELFHeader())
    return false;

  return true;
}

bool ELFDynObjWriter::WriteELFHeader()
{
  cout << "Write ELF Headers" << endl;
  MCLDInfo &ldInfo = m_Linker.getLDInfo();
  MCLDFile &ldFile = ldInfo.output();
  MemoryArea *memArea = ldFile.memArea();

  ScopedWriter *pWriter = new ScopedWriter(memArea->request(0, sizeof(ELF::Elf32_Ehdr)), true);

  /* ELF Identification */
  pWriter->Write8(ELF::ElfMagic[0]);   //EI_MAG0
  pWriter->Write8(ELF::ElfMagic[1]);   //EI_MAG1
  pWriter->Write8(ELF::ElfMagic[2]);   //EI_MAG2
  pWriter->Write8(ELF::ElfMagic[3]);   //EI_MAG3
  pWriter->Write8(ELF::ELFCLASS32);    //EI_CLASS
  pWriter->Write8(ELF::ELFDATA2LSB);   //EI_DATA
  pWriter->Write8(ELF::EV_CURRENT);    //EI_VERSION
  pWriter->Write8(0x0);                //EI_OSABI
  pWriter->Write8(0x0);                //EI_ABIVERSION
  for (int i = (ELF::EI_NIDENT - ELF::EI_PAD); i > 0; --i)
    pWriter->Write8(0x0);              //EI_PAD

  pWriter->Write16(ELF::ET_DYN);            //e_type
  pWriter->Write16(target().machine());     //e_machine
  pWriter->Write32(ELF::EV_CURRENT);        //e_version
  //FIXME: it seems pointing to the first .text section
  pWriter->Write32(0x0);                    //e_entry
  //FIXME:
  pWriter->Write32(0x0);                    //e_phoff = program header offset
  //FIXME:
  pWriter->Write32(0x0);                    //e_shoff = section header offset FIXME
  //FIXME: ARM should add enrty point judgement
  if (ELF::EM_ARM == target().machine())    //e_flags
    pWriter->Write32(ELF::EF_ARM_EABIMASK & DefaultEABIVersion);
  else
    pWriter->Write32(0x0);
  pWriter->Write16(sizeof(ELF::Elf32_Ehdr));     //e_ehsize = elf header size

  //FIXME
  pWriter->Write16(0x0);                    //e_phentsize
  pWriter->Write16(0x0);                    //e_phnum

  //FIXME
  pWriter->Write16(sizeof(ELF::Elf32_Shdr));  //e_shentsize
  pWriter->Write16(0x0);                      //e_shnum
  pWriter->Write16(ELF::SHN_UNDEF);           //e_shstrndx

  delete pWriter;

  return true;
}
