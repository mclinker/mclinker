; RUN: %MCLinker -z relro -march=mips64el                \
; RUN: -dynamic-linker /lib64/ld.so.1                    \
; RUN: %p/../libs/MIPS/Linux/64/crt1.o                   \
; RUN: %p/../libs/MIPS/Linux/64/crti.o                   \
; RUN: %p/../libs/MIPS/Linux/64/crtbegin.o               \
; RUN: %p/irix6_ar/archive_main.o                        \
; RUN: %p/irix6_ar/irix6_archive_all.a                   \
; RUN: %p/../libs/MIPS/Linux/64/crtend.o                 \
; RUN: %p/../libs/MIPS/Linux/64/crtn.o                   \
; RUN: %p/../libs/MIPS/Linux/64/libc.so.6                \
; RUN: %p/../libs/MIPS/Linux/64/libc_nonshared.a         \
; RUN: --as-needed                                       \
; RUN: %p/../libs/MIPS/Linux/64/ld.so.1                  \
; RUN: --no-as-needed                                    \
; RUN: %p/../libs/MIPS/Linux/64/libgcc_s.so.1            \
; RUN: %p/../libs/MIPS/Linux/64/libm.so.6                \
; RUN: %p/../libs/MIPS/Linux/64/libstdc++.so.6 -o %t.out

; RUN: readelf -s %t.out | awk '{print $8}' | FileCheck %s
; CHECK: archive_test1
; CHECK: archive_test2
; CHECK: archive_test3
; CHECK: archive_test4
; CHECK-NOT: archive_test5
