; RUN: %LLC -mtriple="arm-none-linux-gnueabi" -march=arm \
; RUN: -filetype=obj -relocation-model=pic %s -o %t.o
; RUN: %MCLinker -mtriple="arm-none-linux-gnueabi" \
; RUN: --build-id -o %t.arm.exe %t.o -pie
; RUN: test -f %t.arm.exe

target triple = "arm-none-linux-gnueabi"

@.str = private unnamed_addr constant [911 x i8] c"\0A                      /\5C   _ /     \0A                  \5C._\5C`.\5C`-./      \0A             -._ /  `. `   (       \0A             _`/`< _  `-. __\5C      \0A             `.  /  )    ( @)(     \0A             / ) `-./`_.-`._  `._  \0A             _/< __( (      `-._ ) \0A             `._)   \5C `.        v \0A              / \5C__  \5C  \5C       \0A            _/_ / `` -\5C -\5C       \0A             \5C   \5C-..._\5C  \5C    \0A           . | _ /_     \5C.-\5C     \0A          / `.\5C  \5C ``-.. :  :    \0A         /`. `---+|_    `|-.|      \0A         | / _.._/  ``-. |  |      \0A         |`. .-/ /-.._  ` - :      \0A          \5C/..`\5C      `..._/     \0A           `.   |--.._( +-.        \0A                |    (`/           \0A               /__.-:`/-.-._       \0A               )    |:.._.. `.     \0A               :_.-.||.. `... \5C   \0A                )_.-::`...\5C/-.(   \0A                 )  .`:._..`. /    \0A                  `:   /  \5C  /    \0A                    `-/----.\00", align 1
@str = global i8* getelementptr inbounds ([911 x i8]* @.str, i32 0, i32 0), align 8

define i8* @f(i32 %c) nounwind uwtable ssp {
entry:
  %c.addr = alloca i32, align 4
  store i32 %c, i32* %c.addr, align 4
  %0 = load i8** @str, align 8
  %1 = load i32* %c.addr, align 4
  %idx.ext = zext i32 %1 to i64
  %add.ptr = getelementptr inbounds i8* %0, i64 %idx.ext
  ret i8* %add.ptr
}
