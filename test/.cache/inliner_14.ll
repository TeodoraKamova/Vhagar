; ModuleID = 'compile/inliner_14.c'
source_filename = "compile/inliner_14.c"

define i32 @f(i32 %0) {
  ret i32 %0
}

define i32 @main() {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %0 = sub i32 %x.0, 1
  br label %licm-header

1:                                                ; preds = %licm-header
  ret i32 %x.0

licm-header:                                      ; preds = %entry, %licm-latch
  %x.0 = phi i32 [ %0, %licm-latch ], [ 15, %entry ]
  %2 = call i32 @f(i32 %x.0)
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %licm-latch, label %1
}
