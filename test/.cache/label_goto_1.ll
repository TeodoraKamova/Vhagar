; ModuleID = 'compile/label_goto_1.c'
source_filename = "compile/label_goto_1.c"

define i32 @main(i32 %0, ptr %1) {
entry:
  br label %licm-header

licm-header:                                      ; preds = %entry, %licm-latch
  %x.0 = phi i32 [ %4, %licm-latch ], [ 0, %entry ]
  %2 = icmp sge i32 %x.0, %0
  br i1 %2, label %3, label %licm-latch

3:                                                ; preds = %licm-header
  ret i32 %0

licm-latch:                                       ; preds = %licm-header
  %4 = add i32 %x.0, 1
  br label %licm-header
}
