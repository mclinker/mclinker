; Build the shared library.
; RUN: %MCLinker -Bsymbolic --eh-frame-hdr \
; RUN: -dynamic-linker /system/bin/linker64 \
; RUN: -march=aarch64 -mtriple="aarch64-none-linux-gnueabi" \
; RUN: -o hello.out \
; RUN: -L=%p/../../libs/AArch64/Android/ \
; RUN: %p/../../libs/AArch64/Android/crtbegin_dynamic.o \
; RUN: %p/hello.o -z noexecstack -z relro -z now \
; RUN: -lm -lc -ldl -lgcc \
; RUN: %p/../../libs/AArch64/Android/crtend_android.o

; RUN: diff -s hello.out %p/hello.golden.out
