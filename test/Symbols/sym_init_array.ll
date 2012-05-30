; RUN: ld.gold -z relro -m elf_i386 \
; RUN: %p/../libs/X86/Linux/crt1.o %p/../libs/X86/Linux/crti.o \
; RUN: %p/main_empty.o %p/../libs/X86/Linux/libc_nonshared/elf-init.oS \
; RUN: %p/../libs/X86/Linux/crtn.o \
; RUN: %p/../libs/X86/Linux/libc.so.6 -o %t.out

; RUN: readelf -s %t.out | grep __init_array_start | grep ABS
; RUN: readelf -s %t.out | grep __init_array_end | grep ABS
