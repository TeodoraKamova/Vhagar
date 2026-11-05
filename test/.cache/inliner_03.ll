; ModuleID = 'compile/inliner_03.c'
source_filename = "compile/inliner_03.c"

define i32 @f(i32 %0) {
  %2 = and i32 %0, 1
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %4, label %8

4:                                                ; preds = %1
  %5 = shl i32 %0, 2
  %6 = sub i32 %5, %0
  %7 = add i32 %6, 1
  br label %12

8:                                                ; preds = %1
  %9 = lshr i32 %0, 31
  %10 = add i32 %0, %9
  %11 = ashr i32 %10, 1
  br label %12

12:                                               ; preds = %8, %4
  %.0 = phi i32 [ %7, %4 ], [ %11, %8 ]
  ret i32 %.0
}

define i32 @main() {
  %1 = tail call i32 @f(i32 0)
  ret i32 %1
}
