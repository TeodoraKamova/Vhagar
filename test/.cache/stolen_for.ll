; ModuleID = 'compile/stolen_for.c'
source_filename = "compile/stolen_for.c"

define i32 @main() {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %0 = sub i32 %x.0, 1
  br label %licm-header

1:                                                ; preds = %licm-header
  %2 = icmp ne i32 %x.0, 0
  br i1 %2, label %4, label %5

licm-header:                                      ; preds = %entry, %licm-latch
  %x.0 = phi i32 [ %0, %licm-latch ], [ 10, %entry ]
  %3 = icmp ne i32 %x.0, 0
  br i1 %3, label %licm-latch, label %1

4:                                                ; preds = %1
  ret i32 1

5:                                                ; preds = %1
  br label %licm-header3

licm-latch4:                                      ; preds = %licm-header3
  %6 = sub i32 %x.1, 1
  br label %licm-header3

7:                                                ; preds = %licm-header3
  ret i32 %x.1

licm-header3:                                     ; preds = %5, %licm-latch4
  %x.1 = phi i32 [ %6, %licm-latch4 ], [ 10, %5 ]
  %8 = icmp ne i32 %x.1, 0
  br i1 %8, label %licm-latch4, label %7
}
