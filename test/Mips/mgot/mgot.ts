; RUN: %MCLinker -march mipsel -mtriple=mipsel-none-linux-gnueabi \
; RUN:           -filetype=dso -shared -fPIC \
; RUN:           -o %t.so %p/mgot0.o %p/mgot1.o
;
; RUN: readelf -a %t.so | FileCheck %s -check-prefix=MGOT
;
; Check GOT section size
; MGOT: .got PROGBITS {{[0-9a-fA-F]+}} {{[0-9a-fA-F]+}} 010178
;
; Check GOT elements numbers
; MGOT: (MIPS_LOCAL_GOTNO)   4
; MGOT: (MIPS_GOTSYM)      0x7
;
; Check .rel.dyn size
; MGOT: Relocation section '.rel.dyn' {{.*}} contains 87 entries
;
; Check .rel.dyn content
; MGOT: {{[0-9a-fA-F]+}} {{[0-9a-fA-F]+}} R_MIPS_REL32
; MGOT: {{[0-9a-fA-F]+}} {{[0-9a-fA-F]+}} R_MIPS_REL32 00000000 printf
; MGOT: {{[0-9a-fA-F]+}} {{[0-9a-fA-F]+}} R_MIPS_REL32 00000000 foo16300
; MGOT: {{[0-9a-fA-F]+}} {{[0-9a-fA-F]+}} R_MIPS_REL32 00000000 foo16383
; MGOT: {{[0-9a-fA-F]+}} {{[0-9a-fA-F]+}} R_MIPS_REL32
;
; Check .dynsym size
; MGOT: Symbol table '.dynsym' contains 16392 entries
;
; Check .dynsym content
; MGOT: 7: 00000000 0 NOTYPE GLOBAL DEFAULT UND printf
; MGOT: 8: 00000000 0 NOTYPE GLOBAL DEFAULT UND foo00000
; MGOT: 9: 00000000 0 NOTYPE GLOBAL DEFAULT UND foo00001
;
; Check GOT content
; MGOT: Primary GOT:
; MGOT:  Canonical gp value: {{[0-9a-fA-F]+}}
; MGOT:  Reserved entries:
; MGOT:   {{[0-9a-fA-F]+}} -32752(gp) 00000000 Lazy resolver
; MGOT:   {{[0-9a-fA-F]+}} -32748(gp) 80000000 Module pointer (GNU extension)
; MGOT:  Local entries:
; MGOT:   {{[0-9a-fA-F]+}} -32744(gp) 00100000
; MGOT:   {{[0-9a-fA-F]+}} -32740(gp) 00100000
; MGOT:  Global entries:
; MGOT:   {{[0-9a-fA-F]+}} -32736(gp) 00000000 00000000 NOTYPE  UND printf
; MGOT:   {{[0-9a-fA-F]+}} -32732(gp) 00000000 00000000 NOTYPE  UND foo00000
; MGOT:   {{[0-9a-fA-F]+}}  32760(gp) 00000000 00000000 NOTYPE  UND foo16373
; MGOT:   {{[0-9a-fA-F]+}}  32764(gp) 00000000 00000000 NOTYPE  UND foo16374
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16375
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16376
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16377
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16378
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16379
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16380
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16381
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16382
; MGOT:   {{[0-9a-fA-F]+}}            00000000 00000000 NOTYPE  UND foo16383
