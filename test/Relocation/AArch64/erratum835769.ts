; RUN: %MCLinker -mtriple=aarch64-linux-android %p/erratum835769.o \
; RUN: -o %t -shared --fix-cortex-a53-835769
; RUN: readelf -sW %t | FileCheck %s
; CHECK: __erratum_835769_veneer0@island-0
; CHECK: __erratum_835769_veneer1@island-0
; CHECK: __erratum_835769_veneer2@island-0
