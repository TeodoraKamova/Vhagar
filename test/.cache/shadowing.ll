; ModuleID = 'compile/shadowing.c'
source_filename = "compile/shadowing.c"

define i32 @main() {
  %res = alloca i32, align 4
  store i32 0, ptr %res, align 4
  store i8 0, ptr %res, align 1
  %1 = load i8, ptr %res, align 1
  %2 = sext i8 %1 to i32
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  ret i32 1

5:                                                ; preds = %0
  store i32 256, ptr %res, align 4
  %6 = load i32, ptr %res, align 4
  %7 = icmp ne i32 %6, 256
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  ret i32 2

9:                                                ; preds = %5
  %10 = load i32, ptr %res, align 4
  ret i32 %10
}
