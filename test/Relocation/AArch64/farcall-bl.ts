; RUN: %MCLinker -mtriple=aarch64-linux-android %p/farcall-bl.o \
; RUN: -o %t --section-start=.foo=0x8401000
; RUN: readelf -sW %t | FileCheck %s
; CHECK: __bar_adrp_veneer@island-0
