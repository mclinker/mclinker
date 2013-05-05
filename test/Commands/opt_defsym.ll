; RUN: %MCLinker  -march=x86 -mtriple=i386-none-linux-gnueabi \
; RUN: --defsym boo=foo+2 --defsym zoo=bar-main+10\
; RUN: -o %t %p/../libs/X86/Linux/defsym.o
; RUN: readelf -s %t | FileCheck %s
; CHECK: 08049006     0 NOTYPE  GLOBAL DEFAULT  ABS boo
; CHECK: 00000f5e     0 NOTYPE  GLOBAL DEFAULT  ABS zoo
