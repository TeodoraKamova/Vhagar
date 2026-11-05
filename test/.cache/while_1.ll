; ModuleID = 'compile/while_1.c'
source_filename = "compile/while_1.c"

define i32 @main(i32 %0, ptr %1) {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %2 = add i32 %counter.0, 1
  %3 = sub i32 %i.0, 1
  br label %licm-header

4:                                                ; preds = %licm-header
  %5 = add i32 %counter.0, 1
  ret i32 %5

licm-header:                                      ; preds = %entry, %licm-latch
  %i.0 = phi i32 [ %3, %licm-latch ], [ %0, %entry ]
  %counter.0 = phi i32 [ %2, %licm-latch ], [ 0, %entry ]
  %6 = icmp ne i32 %i.0, 0
  br i1 %6, label %licm-latch, label %4
}
