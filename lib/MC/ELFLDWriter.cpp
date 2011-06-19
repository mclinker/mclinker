
void ELFLDWriter::CreateRelocationSections(MCAssemble &Asm,
                                           MCAsmLayout &Layout,
                                           RelMapTy &RelMap) {


    std::string RelaSectionName = hasRelocationAddend() : ".rela" : ".rel";
    RelaSectionName += SectionName;

    unsigned EntrySize;
    if (hasRelocationAddend())
      EntrySize = is64Bit() ? sizeof(ELF::Elf64_Rela) : sizeof(ELF::Elf32_Rela);
    else
      EntrySize = is64Bit() ? sizeof(ELF::Elf64_Rel) : sizeof(ELF::Elf32_Rel);

    const MCSection *RelaSection =
      Ctx.getELFSection(RelSectionName, hasRelocationAddend() ?
                        ELF::SHT_RELA : ELF::SHT::REL, 0,
                        Section::getReadOnly(),
                        EntrySize, "");
    RelMap[&Section] = RelaSection;
    Asm.getOrCreateSectionData(*RelaSection);
  

}


void ELFLDWriter::CreateMetadataSections(MCAssembler &Asm,
                                         MCAsmLayout &Layout,
                                         SectionIndexMapTy &SectionIndexMap,
                                         const RelMapTy &RelMap) {



}


void ELFLDWriter::WriteObject(MCAssembler &Asm,
                              const MCAsmLayout &Layout) {





}
