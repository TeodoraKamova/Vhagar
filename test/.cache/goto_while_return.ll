; ModuleID = 'compile/goto_while_return.c'
source_filename = "compile/goto_while_return.c"

define i32 @main() {
entry:
  br label %licm-header

licm-header:                                      ; preds = %entry, %licm-latch
  %a.0 = phi i32 [ 4096, %licm-latch ], [ 1024, %entry ]
  %0 = icmp ne i32 %a.0, 4096
  br i1 %0, label %licm-latch, label %1

licm-latch:                                       ; preds = %licm-header
  br label %licm-header

1:                                                ; preds = %licm-header
  ret i32 0
}
