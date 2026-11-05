; ModuleID = 'compile/inliner_07.c'
source_filename = "compile/inliner_07.c"

define i32 @f(i32 %0) {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %1 = sub i32 %.0, 1
  br label %licm-header

2:                                                ; preds = %licm-header
  ret i32 %.0

licm-header:                                      ; preds = %entry, %licm-latch
  %.0 = phi i32 [ %1, %licm-latch ], [ %0, %entry ]
  %3 = icmp ne i32 %.0, 0
  br i1 %3, label %licm-latch, label %2
}

define i32 @main() {
  %1 = tail call i32 @f(i32 16)
  ret i32 %1
}
