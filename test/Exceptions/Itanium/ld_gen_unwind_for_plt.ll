; Function Attrs: uwtable
define void @_Z3barv() #0 {
entry:
  call void @_Z3foov()
  ret void
}

declare void @_Z3foov() #1

attributes #0 = { uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }
attributes #1 = { "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

; Check x86
; RUN: %LLC -relocation-model=pic -filetype=obj -march=x86 %s -o %t.o
; RUN: %MCLinker -shared -fPIC -march=x86 %t.o -o %t.out

; RUN: readelf -S %t.out | grep '\ \.plt' \
; RUN: | awk '{print "PLT Addr "$5" Align "$12}' > %t.txt
; RUN: readelf -S %t.out | grep -A1 '\ \.plt' | tail -n 1 \
; RUN: | awk '{print "PLT End "$5}' >> %t.txt
; RUN: readelf -S %t.out | grep '\ \.eh_frame' \
; RUN: | awk '{print "EhFrame Size "$7" Align "$12}' >> %t.txt
; RUN: readelf -w %t.out | grep 'pc=' | head -n 1 | awk -F '=' '{print $3}' \
; RUN: | awk -F '.' '{print "PC_START "$1}' >> %t.txt
; RUN: readelf -w %t.out | grep 'pc=' | head -n 1 \
; RUN: | awk -F '.' '{print "PC_END "$3}' >> %t.txt

; RUN: cat %t.txt | FileCheck %s --check-prefix=X86
; X86: PLT Addr [[PLT_ADDR:[0-9a-f]+]] Align 16
; X86-NEXT: PLT End [[PLT_END:[0-9a-f]+]]
; X86-NEXT: EhFrame Size 00005c Align 4
; X86-NEXT: PC_START [[PLT_ADDR]]
; X86-NEXT: PC_END [[PLT_END]]


; Check x86-64
; RUN: %LLC -relocation-model=pic -filetype=obj -march=x86-64 %s -o %t.o
; RUN: %MCLinker -shared -fPIC -march=x86-64 %t.o -o %t.out

; RUN: readelf -S %t.out | grep -A 1 '\ \.plt' | tr '\n' ' ' \
; RUN: | awk '{print "PLT Addr "$5" Align "$12}' > %t.txt
; RUN: readelf -S %t.out | grep -A 2 '\ \.plt' | tail -n 1 \
; RUN: | awk '{print "PLT End "$5}' >> %t.txt
; RUN: readelf -S %t.out | grep -A 1 '\ \.eh_frame' | tr '\n' ' ' \
; RUN: | awk '{print "EhFrame Size "$7" Align "$12}' >> %t.txt
; RUN: readelf -w %t.out | grep 'pc=' | head -n 1 | awk -F '=' '{print $3}' \
; RUN: | awk -F '.' '{printf "PC_START %016d\n", $1}' >> %t.txt
; RUN: readelf -w %t.out | grep 'pc=' | head -n 1 \
; RUN: | awk -F '.' '{printf "PC_END %016d\n", $3}' >> %t.txt

; RUN: cat %t.txt | FileCheck %s --check-prefix=X86_64
; X86_64: PLT Addr [[PLT_ADDR:[0-9a-f]+]] Align 16
; X86_64-NEXT: PLT End [[PLT_END:[0-9a-f]+]]
; X86_64-NEXT: EhFrame Size 0000000000000060 Align 8
; X86_64-NEXT: PC_START [[PLT_ADDR]]
; X86_64-NEXT: PC_END [[PLT_END]]


; Check --no-ld-generated-unwind-info
; RUN: %MCLinker -shared -fPIC -march=x86-64 %t.o -o %t.out --no-ld-generated-unwind-info
; RUN: readelf -w %t.out | grep 'pc=' | wc -l > %t.txt
; RUN: cat %t.txt | FileCheck %s --check-prefix=NO_GEN
; NO_GEN: 1
