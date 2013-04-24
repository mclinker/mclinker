; Build the shared library.
; RUN: %MCLinker -shared -march=hexagon %p/../libs/Hexagon/v4objs/shlib.o\
; RUN: -o simple-dynlib.so
; RUN: diff -s simple-dynlib.so %p/../libs/Hexagon/v4dynlibs/simple-dynlib.so
