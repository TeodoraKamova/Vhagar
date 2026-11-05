; ModuleID = 'compile/global_var.c'
source_filename = "compile/global_var.c"

@g = common global i32 0, align 4

define i32 @foo() {
  ret i32 0
}

define i32 @bar() {
  store i32 0, ptr @g, align 4
  ret i32 0
}

define i32 @main() {
  store i32 1, ptr @g, align 4
  %1 = call i32 @foo()
  %2 = load i32, ptr @g, align 4
  %3 = icmp ne i32 %2, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  ret i32 1

5:                                                ; preds = %0
  %6 = call i32 @bar()
  %7 = load i32, ptr @g, align 4
  ret i32 %7
}
