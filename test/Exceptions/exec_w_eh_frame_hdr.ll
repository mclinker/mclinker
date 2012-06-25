; RUN: %MCLinker --eh-frame-hdr -z relro \
; RUN: -dynamic-linker %p/../libs/X86/Linux/ld-linux.so.2 \
; RUN: -march=x86 %p/../libs/X86/Linux/crt1.o \
; RUN: %p/../libs/X86/Linux/crti.o %p/../libs/X86/Linux/crtbegin.o \
; RUN: %p/exception_main.o %p/../libs/X86/Linux/crtend.o \
; RUN: %p/../libs/X86/Linux/crtn.o %p/../libs/X86/Linux/libc.so.6 \
; RUN: %p/../libs/X86/Linux/libc_nonshared/elf-init.oS \
; RUN: %p/../libs/X86/Linux/libgcc_s.so.1 \
; RUN: %p/../libs/X86/Linux/libm.so.6 \
; RUN: %p/../libs/X86/Linux/libstdc++.so.6 -o %t.out

; RUN: readelf -S %t.out > %t.txt
; RUN: readelf -l %t.out >> %t.txt
; RUN: cat %t.txt | awk 'BEGIN {EH_HDR_ADDR = -1; EH_SEG_ADDR =-2;}  \
; RUN: {if ($2 == ".eh_frame_hdr" && EH_HDR_ADDR == -1)               \
; RUN:   EH_HDR_ADDR = hexstr_to_decnum($4);}     \
; RUN: {if ($1 == "GNU_EH_FRAME")                 \
; RUN:   EH_SEG_ADDR = hexstr_to_decnum($3);}     \
; RUN: END {if (EH_HDR_ADDR != EH_SEG_ADDR) {     \
; RUN:        exit -1;}}                          \
; RUN: function hexstr_to_decnum(str)             \
; RUN: {                                          \
; RUN:   str = tolower(str);                      \
; RUN:   if (substr(str, 1, 2) == "0x")           \
; RUN:     str = substr(str, 3);                  \
; RUN:   n = length(str);                         \
; RUN:   num = 0;                                 \
; RUN:   for (i = 1; i <= n; i++) {               \
; RUN:     c = substr(str, i, 1);                 \
; RUN:     if ((k = index("0123456789", c)) > 0)  \
; RUN:       k--;                                 \
; RUN:     else if ((k = index("abcdef", c)) > 0) \
; RUN:       k += 9;                              \
; RUN:     else {                                 \
; RUN:       print "The input string is not a legal hex number."; \
; RUN:       exit 1;                              \
; RUN:     }                                      \
; RUN:   num = num * 16 + k;                      \
; RUN:   }                                        \
; RUN: return num;                                \
; RUN: }                                          \
; RUN: '
