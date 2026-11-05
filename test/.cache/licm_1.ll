; ModuleID = 'compile/licm_1.c'
source_filename = "compile/licm_1.c"

define i32 @f(i32 %0, i32 %1, i32 %2) {
entry:
  %3 = sub i32 %0, %1
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %4 = mul i32 %i.0, %3
  %5 = add i32 %s.0, %4
  %6 = add i32 %i.0, 1
  br label %licm-header

7:                                                ; preds = %licm-header
  ret i32 %s.0

licm-header:                                      ; preds = %entry, %licm-latch
  %s.0 = phi i32 [ %5, %licm-latch ], [ 0, %entry ]
  %i.0 = phi i32 [ %6, %licm-latch ], [ 0, %entry ]
  %8 = icmp slt i32 %i.0, %2
  br i1 %8, label %licm-latch, label %7
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
