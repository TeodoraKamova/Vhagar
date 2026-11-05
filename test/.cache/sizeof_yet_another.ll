; ModuleID = 'compile/sizeof_yet_another.c'
source_filename = "compile/sizeof_yet_another.c"

@0 = private unnamed_addr constant [19 x i8] c"%d %d %d %d %d %d\0A\00", align 1

declare void @c4_print_int(i32)

declare void @c4_print_str(ptr)

define i32 @main() {
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 4, i32 1, i32 8, i32 8, i32 12, i32 8)
  ret i32 0
}

declare i32 @printf(ptr, ...)
