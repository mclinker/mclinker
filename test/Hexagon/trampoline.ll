; Build the shared library.
; RUN: %MCLinker -march=hexagon %p/../libs/Hexagon/v3objs/trampoline.o\
; RUN:  -o %t
; RUN: diff -s %t %p/../libs/Hexagon/v3execs/trampoline
