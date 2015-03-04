
target triple = "arm-none-linux-gnueabi"

define i8* @__wrap_f(i32 %c) nounwind uwtable ssp {
entry:
  %c.addr = alloca i32, align 4
  store i32 %c, i32* %c.addr, align 4
  %0 = load i32, i32* %c.addr, align 4
  %call = call i8* @__real_f(i32 %0)
  ret i8* %call
}

declare i8* @__real_f(i32)
