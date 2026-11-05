; ModuleID = 'compile/inliner_08.c'
source_filename = "compile/inliner_08.c"

define i32 @f(i32 %0) {
entry:
  br label %licm-header

licm-header:                                      ; preds = %entry, %licm-latch
  %x.0 = phi i32 [ %x.1, %licm-latch ], [ %0, %entry ]
  %1 = icmp eq i32 %x.0, 1
  br i1 %1, label %2, label %3

2:                                                ; preds = %licm-header
  ret i32 %0

3:                                                ; preds = %licm-header
  %4 = and i32 %x.0, 1
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %10

6:                                                ; preds = %3
  %7 = shl i32 %x.0, 2
  %8 = sub i32 %7, %x.0
  %9 = add i32 %8, 1
  br label %licm-latch

10:                                               ; preds = %3
  %11 = lshr i32 %x.0, 31
  %12 = add i32 %x.0, %11
  %13 = ashr i32 %12, 1
  br label %licm-latch

licm-latch:                                       ; preds = %10, %6
  %x.1 = phi i32 [ %9, %6 ], [ %13, %10 ]
  br label %licm-header
}

define i32 @main() {
  %1 = tail call i32 @f(i32 37)
  ret i32 %1
}
