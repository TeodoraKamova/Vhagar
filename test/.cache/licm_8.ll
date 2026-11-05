; ModuleID = 'compile/licm_8.c'
source_filename = "compile/licm_8.c"

define i32 @f(i32 %0, i32 %1, i32 %2) {
  %4 = icmp eq i32 %0, %1
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  br label %7

6:                                                ; preds = %3
  br label %cond

7:                                                ; preds = %12, %5
  %s.0 = phi i32 [ 0, %5 ], [ %s.1, %12 ]
  %i.0 = phi i32 [ 0, %5 ], [ %i.1, %12 ]
  %8 = sub i32 %0, %1
  %9 = mul i32 %i.0, %8
  %10 = add i32 %s.0, %9
  %11 = add i32 %i.0, 1
  br label %cond

12:                                               ; preds = %cond
  br label %7

13:                                               ; preds = %cond
  ret i32 %s.1

cond:                                             ; preds = %7, %6
  %s.1 = phi i32 [ %10, %7 ], [ 0, %6 ]
  %i.1 = phi i32 [ %11, %7 ], [ 0, %6 ]
  %14 = icmp slt i32 %i.1, %2
  br i1 %14, label %12, label %13
}

define i32 @main() {
  %1 = call i32 @f(i32 2, i32 3, i32 4)
  %2 = ashr i32 %1, 7
  %3 = lshr i32 %2, 24
  %4 = add i32 %1, %3
  %5 = and i32 %4, -256
  %6 = sub i32 %1, %5
  ret i32 %6
}
