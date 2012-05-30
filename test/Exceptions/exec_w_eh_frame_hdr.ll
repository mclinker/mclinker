; XFAIL:*

; RUN: %MCLinker --eh-frame-hdr -z relro \
; RUN: -dynamic-linker %p/../libs/X86/Linux/ld-linux.so.2 \
; RUN: -march=x86 %p/../libs/X86/Linux/crt1.o \
; RUN: %p/../libs/X86/Linux/crti.o %p/../libs/X86/Linux/crtbegin.o \
; RUN: %p/exception_main.o %p/../libs/X86/Linux/crtend.o \
; RUN: %p/../libs/X86/Linux/crtn.o %p/../libs/X86/Linux/libc.so.6 \
; RUN: %p/../libs/X86/Linux/libc_nonshared/*.oS \
; RUN: -L=%p/../libs/X86/Linux -lm -lstdc++ -lgcc_s -o %t.out

; RUN: %t.out 2> %t.output
; RUN: grep 'catch exception' %t.output
