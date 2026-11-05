; ModuleID = 'compile/inliner_15.c'
source_filename = "compile/inliner_15.c"

define i32 @f(i32 %0) {
  ret i32 %0
}

define i32 @main(i32 %0, ptr %1) {
  %3 = call i32 @f(i32 %0)
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %8

5:                                                ; preds = %2
  %6 = call i32 @f(i32 %0)
  %7 = sub i32 %6, 1
  br label %11

8:                                                ; preds = %2
  %9 = mul i32 %0, 1
  %10 = call i32 @f(i32 %9)
  br label %11

11:                                               ; preds = %8, %5
  %.0 = phi i32 [ %7, %5 ], [ %10, %8 ]
  ret i32 %.0
}
