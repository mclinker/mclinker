; RUN: %MCLinker -march=hexagon %p/../libs/Hexagon/v4objs/sdata.o\
; RUN:  -G 8 -o %t -mtriple=hexagon-none-linux
; RUN: llvm-nm -n %t | FileCheck %s -check-prefix=SORTORDER

SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} A1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} AA1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} a1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} B1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} BB1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} b1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} C1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} CC1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} c1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} D1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} DD1
SORTORDER: {{[0-9a-f]+}} {{[?A-Z]}} d1
