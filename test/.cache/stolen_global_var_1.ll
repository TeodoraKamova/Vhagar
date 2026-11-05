; ModuleID = 'compile/stolen_global_var_1.c'
source_filename = "compile/stolen_global_var_1.c"

@x = common global i32 0, align 4

define i32 @main() {
  %1 = load i32, ptr @x, align 4
  %2 = icmp ne i32 %1, 3
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 0

4:                                                ; preds = %0
  store i32 0, ptr @x, align 4
  %5 = load i32, ptr @x, align 4
  ret i32 %5
}

define ptr @foo() {
  ret ptr @main
}
