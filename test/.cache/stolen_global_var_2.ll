; ModuleID = 'compile/stolen_global_var_2.c'
source_filename = "compile/stolen_global_var_2.c"

@x = common global i32 0, align 4

define i32 @main() {
  %1 = load i32, ptr @x, align 4
  ret i32 %1
}
