; Build the shared library.
; RUN: %MCLinker -march=hexagon %p/../libs/Hexagon/v3objs/trampoline.o\
; RUN:  -o %t -mtriple=hexagon-none-linux
; RUN: diff -s %t %p/../libs/Hexagon/v3execs/trampoline
