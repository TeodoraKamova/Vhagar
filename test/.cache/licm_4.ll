; ModuleID = 'compile/licm_4.c'
source_filename = "compile/licm_4.c"

@i_feel_like_it = common global i32 0, align 4

define i32 @f(i32 %0, i32 %1, i32 %2) {
entry:
  %3 = load i32, ptr @i_feel_like_it, align 4
  %4 = icmp ne i32 %3, 0
  br i1 %4, label %5, label %7

5:                                                ; preds = %entry
  %6 = sub i32 %0, %1
  br label %licm-header

7:                                                ; preds = %11, %entry
  %s.0 = phi i32 [ %s.1, %11 ], [ 0, %entry ]
  ret i32 %s.0

licm-latch:                                       ; preds = %licm-header
  %8 = mul i32 %i.0, %6
  %9 = add i32 %s.1, %8
  %10 = add i32 %i.0, 1
  br label %licm-header

11:                                               ; preds = %licm-header
  br label %7

licm-header:                                      ; preds = %5, %licm-latch
  %s.1 = phi i32 [ %9, %licm-latch ], [ 0, %5 ]
  %i.0 = phi i32 [ %10, %licm-latch ], [ 0, %5 ]
  %12 = icmp slt i32 %i.0, %2
  br i1 %12, label %licm-latch, label %11
}

define i32 @main() {
  store i32 1, ptr @i_feel_like_it, align 4
  %1 = call i32 @f(i32 2, i32 3, i32 4)
  %2 = ashr i32 %1, 7
  %3 = lshr i32 %2, 24
  %4 = add i32 %1, %3
  %5 = and i32 %4, -256
  %6 = sub i32 %1, %5
  ret i32 %6
}
