; RUN: %MCLinker -mtriple=mipsel-none-linux-gnueabi \
; RUN: --eh-frame-hdr -EL -m elf32ltsmip -shared -Bsymbolic \
; RUN: -z noexecstack -z relro -z now \
; RUN: --no-undefined -z noexecstack -z relro -z now \
; RUN: -o %t.so \
; RUN: -soname=libhello-jni.so \
; RUN: --sysroot=%p/../../libs/MIPS/Android/android-14 \
; RUN: %p/../../libs/MIPS/Android/android-14/crtbegin_so.o \
; RUN: -L%p/../../libs/MIPS/Android/android-14 \
; RUN: %p/hello-jni.o \
; RUN: -lgcc -lstdc++ -lm -lc -lgcc -lc -ldl -lgcc \
; RUN: %p/../../libs/MIPS/Android/android-14/crtend_so.o
