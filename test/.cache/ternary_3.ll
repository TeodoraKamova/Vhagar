; ModuleID = 'compile/ternary_3.c'
source_filename = "compile/ternary_3.c"

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %4, label %6

4:                                                ; preds = %2
  %5 = add i32 %0, 1
  br label %8

6:                                                ; preds = %2
  %7 = add i32 %0, 1
  br label %8

8:                                                ; preds = %6, %4
  %.0 = phi i32 [ %5, %4 ], [ %7, %6 ]
  %9 = icmp ne i32 %.0, 2
  %10 = zext i1 %9 to i32
  ret i32 %10
}
