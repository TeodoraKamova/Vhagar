; ModuleID = 'compile/decl_global_1.c'
source_filename = "compile/decl_global_1.c"

@x = common global i32 0, align 4

define i32 @main() {
  store i32 5, ptr @x, align 4
  %1 = load i32, ptr @x, align 4
  ret i32 %1
}
