; ModuleID = 'compile/chars.c'
source_filename = "compile/chars.c"

declare i32 @putchar(i32)

define i32 @main() {
  %1 = call i32 @putchar(i32 99)
  %2 = call i32 @putchar(i32 10)
  ret i32 0
}
