; ModuleID = 'compile/chars_overflow.c'
source_filename = "compile/chars_overflow.c"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare void @c4_print_int(i32)

define i32 @main() {
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 128)
  ret i32 0
}

declare i32 @printf(ptr, ...)
