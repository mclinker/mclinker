
target triple = "arm-none-linux-gnueabi"

@.str = private unnamed_addr constant [19 x i8] c"Happy Mother's Day\00", align 1
@str = global i8* getelementptr inbounds ([19 x i8]* @.str, i32 0, i32 0), align 8

define i8* @_Z1fj(i32 %c) nounwind uwtable ssp {
entry:
  %c.addr = alloca i32, align 4
  store i32 %c, i32* %c.addr, align 4
  %0 = load i8** @str, align 8
  %1 = load i32* %c.addr, align 4
  %idx.ext = zext i32 %1 to i64
  %add.ptr = getelementptr inbounds i8* %0, i64 %idx.ext
  ret i8* %add.ptr
}
