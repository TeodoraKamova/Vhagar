; ModuleID = 'compile/struct_reverse_order.c'
source_filename = "compile/struct_reverse_order.c"

@s = common global { i8, i32 } zeroinitializer, align 8
@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare void @c4_print_int(i32)

define i32 @main() {
  store i32 4, ptr getelementptr inbounds ({ i8, i32 }, ptr @s, i32 0, i32 1), align 4
  store i8 2, ptr @s, align 1
  %1 = load i32, ptr getelementptr inbounds ({ i8, i32 }, ptr @s, i32 0, i32 1), align 4
  %2 = call i32 (ptr, ...) @printf(ptr @0, i32 %1)
  %3 = load i8, ptr @s, align 1
  %4 = sext i8 %3 to i32
  %5 = call i32 (ptr, ...) @printf(ptr @0, i32 %4)
  ret i32 0
}

declare i32 @printf(ptr, ...)
