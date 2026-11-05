; ModuleID = 'compile/licm_7.c'
source_filename = "compile/licm_7.c"

define i32 @f(i32 %0, i32 %1, i32 %2) {
entry:
  %3 = sub i32 %0, %1
  br label %licm-header

4:                                                ; preds = %licm-header
  %5 = mul i32 %i.0, %3
  %6 = add i32 %s.1, %5
  %7 = icmp slt i32 %i.0, 0
  br i1 %7, label %10, label %licm-latch

8:                                                ; preds = %10, %licm-header
  %s.0 = phi i32 [ %6, %10 ], [ %s.1, %licm-header ]
  ret i32 %s.0

licm-header:                                      ; preds = %entry, %licm-latch
  %s.1 = phi i32 [ %6, %licm-latch ], [ 0, %entry ]
  %i.0 = phi i32 [ %11, %licm-latch ], [ 0, %entry ]
  %9 = icmp slt i32 %i.0, %2
  br i1 %9, label %4, label %8

10:                                               ; preds = %4
  br label %8

licm-latch:                                       ; preds = %4
  %11 = add i32 %i.0, 1
  br label %licm-header
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
