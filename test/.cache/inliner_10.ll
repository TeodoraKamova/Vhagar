; ModuleID = 'compile/inliner_10.c'
source_filename = "compile/inliner_10.c"

define i32 @fac(i32 %0) {
  %2 = icmp sgt i32 %0, 4
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  br label %5

4:                                                ; preds = %1
  br label %cond

5:                                                ; preds = %8, %3
  %s.0 = phi i32 [ 24, %3 ], [ %s.1, %8 ]
  %i.0 = phi i32 [ 5, %3 ], [ %i.1, %8 ]
  %6 = mul i32 %s.0, %i.0
  %7 = add i32 %i.0, 1
  br label %cond

8:                                                ; preds = %cond
  br label %5

9:                                                ; preds = %cond
  ret i32 %s.1

cond:                                             ; preds = %5, %4
  %s.1 = phi i32 [ %6, %5 ], [ 1, %4 ]
  %i.1 = phi i32 [ %7, %5 ], [ 1, %4 ]
  %10 = icmp sle i32 %i.1, %0
  br i1 %10, label %8, label %9
}

define i32 @main() {
  %1 = tail call i32 @fac(i32 5)
  ret i32 %1
}
