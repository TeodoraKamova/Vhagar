; ModuleID = 'compile/decl_global_2.c'
source_filename = "compile/decl_global_2.c"

@x = common global ptr null, align 8

define i32 @main() {
  %y = alloca i32, align 4
  store ptr %y, ptr @x, align 8
  store i32 5, ptr %y, align 4
  %1 = load ptr, ptr @x, align 8
  %2 = load i32, ptr %1, align 4
  ret i32 %2
}
