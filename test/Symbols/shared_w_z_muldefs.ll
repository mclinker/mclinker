; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -fPIC -dB %s -o %t.1.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -fPIC -dB %s -o %t.2.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: %t.1.o %t.2.o -z muldefs -o %t.so -shared
; RUN: if [ 2 -ne `test -f %t.so && readelf -s %.so | grep _Z1fv | wc -l` ]; then\
; RUN:  echo "the number of symbol _Z1fv is not two!."; \
; RUN:  exit 1; \
; RUN: fi

define i32 @_Z1fv() nounwind uwtable ssp {
  %b = alloca i32, align 4
  store i32 100, i32* %b, align 4
  %1 = load i32* %b, align 4
  ret i32 %1
}
