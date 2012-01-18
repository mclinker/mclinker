The TestSample.o is an 32bit ARM ELF relocatable object, it's wrote by hand. You can copy it, and just rewrite:
  .text     : 0x0040 ~ 0x004F (4 ARM instructions)
  .rel.text : 0x0050 ~ 0x006F (4 rela entries)
  .data     : 0x0070 ~ 0x007F (16 bytes)
  .rela.text: 0x0080 ~ 0x0097 (2 rela entries)
  .symtab   : 0x02F0 ~ 0x032F (last 4 symbols)
