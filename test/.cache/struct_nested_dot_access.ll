; ModuleID = 'compile/struct_nested_dot_access.c'
source_filename = "compile/struct_nested_dot_access.c"

@s = common global { i32, { i32, i32 } } zeroinitializer, align 8

define i32 @main() {
  store i32 9, ptr getelementptr inbounds ({ i32, { i32, i32 } }, ptr @s, i32 0, i32 1), align 4
  store i32 4, ptr @s, align 4
  store i32 6, ptr getelementptr inbounds ({ i32, i32 }, ptr getelementptr inbounds ({ i32, { i32, i32 } }, ptr @s, i32 0, i32 1), i32 0, i32 1), align 4
  %1 = load i32, ptr @s, align 4
  %2 = load i32, ptr getelementptr inbounds ({ i32, { i32, i32 } }, ptr @s, i32 0, i32 1), align 4
  %3 = add i32 %1, %2
  %4 = load i32, ptr getelementptr inbounds ({ i32, i32 }, ptr getelementptr inbounds ({ i32, { i32, i32 } }, ptr @s, i32 0, i32 1), i32 0, i32 1), align 4
  %5 = add i32 %3, %4
  ret i32 %5
}
