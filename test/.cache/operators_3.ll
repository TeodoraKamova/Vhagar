; ModuleID = 'compile/operators_3.c'
source_filename = "compile/operators_3.c"

@0 = private unnamed_addr constant [37 x i8] c"%d\0A%d\0A%d\0A%d %d %d\0A%d %d %d %d %d %d\0A\00", align 1

declare void @c4_print_int(i32)

declare void @c4_print_str(ptr)

define i32 @main() {
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 0, i32 2, i32 7, i32 0, i32 1, i32 0, i32 0, i32 1, i32 0, i32 1, i32 0, i32 1)
  ret i32 6
}

declare i32 @printf(ptr, ...)
