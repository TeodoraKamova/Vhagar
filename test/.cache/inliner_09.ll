; ModuleID = 'compile/inliner_09.c'
source_filename = "compile/inliner_09.c"

define i32 @fac(i32 %0) {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %1 = mul i32 %s.0, %i.0
  %2 = add i32 %i.0, 1
  br label %licm-header

3:                                                ; preds = %licm-header
  ret i32 %s.0

licm-header:                                      ; preds = %entry, %licm-latch
  %s.0 = phi i32 [ %1, %licm-latch ], [ 1, %entry ]
  %i.0 = phi i32 [ %2, %licm-latch ], [ 1, %entry ]
  %4 = icmp sle i32 %i.0, %0
  br i1 %4, label %licm-latch, label %3
}

define i32 @main() {
  %1 = tail call i32 @fac(i32 5)
  ret i32 %1
}
