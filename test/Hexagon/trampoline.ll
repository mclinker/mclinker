; Build the shared library.
; RUN: %MCLinker -march=hexagon %p/../libs/Hexagon/v3objs/trampoline.o\
; RUN:  -o trampoline.tmp
; RUN: diff -s trampoline.tmp %p/../libs/Hexagon/v3execs/trampoline
