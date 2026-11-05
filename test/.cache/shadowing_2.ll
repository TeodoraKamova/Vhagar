; ModuleID = 'compile/shadowing_2.c'
source_filename = "compile/shadowing_2.c"

define i32 @main() {
  %i = alloca i32, align 4
  %res = alloca i32, align 4
  %res1 = alloca i32, align 4
  br label %1

1:                                                ; preds = %0
  store i32 0, ptr %res, align 4
  store i32 0, ptr %i, align 4
  br label %cond

2:                                                ; preds = %cond
  %3 = load i32, ptr %i, align 4
  %4 = add i32 %3, 1
  store i32 %4, ptr %res, align 4
  %5 = load i32, ptr %i, align 4
  %6 = add i32 %5, 1
  store i32 %6, ptr %i, align 4
  br label %cond

7:                                                ; preds = %cond
  %8 = load i32, ptr %res, align 4
  ret i32 %8

cond:                                             ; preds = %2, %1
  %9 = load i32, ptr %i, align 4
  %10 = icmp slt i32 %9, 10
  br i1 %10, label %2, label %7
}
