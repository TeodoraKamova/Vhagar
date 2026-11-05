; ModuleID = 'compile/inliner_04.c'
source_filename = "compile/inliner_04.c"

define i32 @f(i32 %0) {
  %2 = icmp sge i32 %0, 0
  br i1 %2, label %3, label %9

3:                                                ; preds = %1
  %4 = lshr i32 %0, 31
  %5 = add i32 %0, %4
  %6 = and i32 %5, -2
  %7 = sub i32 %0, %6
  %8 = icmp eq i32 %7, 0
  br i1 %8, label %11, label %12

9:                                                ; preds = %1
  %10 = icmp eq i32 %0, -5
  br i1 %10, label %14, label %15

11:                                               ; preds = %3
  ret i32 %0

12:                                               ; preds = %3
  %13 = sub i32 0, %0
  ret i32 %13

14:                                               ; preds = %9
  ret i32 14

15:                                               ; preds = %9
  ret i32 57
}

define i32 @main() {
  %1 = tail call i32 @f(i32 0)
  ret i32 %1
}
