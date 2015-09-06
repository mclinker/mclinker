; Check the output for the unsorted .ARM.exidx inputs.
;
; If a function marked with section attribute comes before the function without
; section attribute (i.e. the function in .text section), then the
; corresponding .ARM.exidx sections will not be sorted in the input object file.
; It is the duty of the linker to sort them properly.
;
; This test will check the order of the .ARM.exidx entries.

; RUN: %LLC -mtriple=armv7-linux-gnueabi -filetype=obj \
; RUN:   -relocation-model=pic %s -o %t.o

; RUN: %MCLinker -shared --eh-frame-hdr -z relro \
; RUN:   -mtriple=armv7-linux-gnueabi -march=arm \
; RUN:   --dynamic-linker=/system/bin/linker \
; RUN:   %p/../../libs/ARM/Android/android-14/crtbegin_so.o \
; RUN:   %t.o \
; RUN:   %p/../../libs/ARM/Android/android-14/crtend_so.o \
; RUN:   -L%p/../../libs/ARM/Android/android-14 \
; RUN:   -L%p/../../libs/ARM/Android/cxx-stl    \
; RUN:   -lstdc++ -lm -lc -lgnustl_shared       \
; RUN:   -o %t.so

; RUN: readelf -u %t.so | FileCheck %s -check-prefix=CHECK

; CHECK: <test2>: 0x808480b0
; CHECK:   Compact model index: 0
; CHECK:   0x84 0x80 pop {r11, r14}
; CHECK:   0xb0      finish

; CHECK: <test1>: 0x808480b0
; CHECK:   Compact model index: 0
; CHECK:   0x84 0x80 pop {r11, r14}
; CHECK:   0xb0      finish

; CHECK: <test1+0x{{[0-9a-fA-F]+}}>: 0x1 [cantunwind]

; RUN: rm %t.o %t.so

target datalayout = "e-m:e-p:32:32-i64:64-v128:64:128-a:0:32-n32-S64"
target triple = "armv7--linux-gnueabihf"

@_ZTIi = external constant i8*

declare i8* @__cxa_allocate_exception(i32)

declare void @__cxa_throw(i8*, i8*, i8*)

define i32 @test1(i32 %cond) section ".text.test1" {
entry:
  %tobool = icmp eq i32 %cond, 0
  br i1 %tobool, label %if.end, label %if.then

if.then:
  %exception = tail call i8* @__cxa_allocate_exception(i32 4)
  %0 = bitcast i8* %exception to i32*
  store i32 0, i32* %0, align 4
  tail call void @__cxa_throw(i8* %exception, i8* bitcast (i8** @_ZTIi to i8*), i8* null) #2
  unreachable

if.end:
  ret i32 0
}

define i32 @test2(i32 %a, i32 %b, i32 %c) section ".text" {
entry:
  %a2 = mul i32 %a, %a
  %b2 = mul i32 %b, %b
  %c2 = mul i32 %c, %c
  %a2.b2 = add i32 %a2, %b2
  %tobool = icmp eq i32 %a2.b2, %c2
  br i1 %tobool, label %if.then, label %if.end

if.then:
  %exception = tail call i8* @__cxa_allocate_exception(i32 4)
  %0 = bitcast i8* %exception to i32*
  store i32 0, i32* %0, align 4
  tail call void @__cxa_throw(i8* %exception, i8* bitcast (i8** @_ZTIi to i8*), i8* null) #2
  unreachable

if.end:
  ret i32 0
}
