; ModuleID = 'compile/break_and_continue.c'
source_filename = "compile/break_and_continue.c"

define i32 @main() {
entry:
  br label %licm-header3

0:                                                ; preds = %licm-header3
  %1 = icmp sge i32 %i.0, 10
  br i1 %1, label %5, label %licm-latch4

2:                                                ; preds = %5, %licm-header3
  %3 = icmp ne i32 %i.0, 10
  br i1 %3, label %7, label %8

licm-header3:                                     ; preds = %entry, %licm-latch4
  %i.0 = phi i32 [ %6, %licm-latch4 ], [ 0, %entry ]
  %4 = icmp slt i32 %i.0, 20
  br i1 %4, label %0, label %2

5:                                                ; preds = %0
  br label %2

licm-latch4:                                      ; preds = %0
  %6 = add i32 %i.0, 1
  br label %licm-header3

7:                                                ; preds = %2
  ret i32 1

8:                                                ; preds = %2
  br label %licm-header

9:                                                ; preds = %licm-header
  %10 = sub i32 %i.1, 1
  %11 = icmp slt i32 %10, 50
  br i1 %11, label %licm-latch, label %14

12:                                               ; preds = %licm-header
  ret i32 %i.1

licm-header:                                      ; preds = %8, %licm-latch
  %i.1 = phi i32 [ %10, %licm-latch ], [ %i.0, %8 ]
  %13 = icmp sgt i32 %i.1, 0
  br i1 %13, label %9, label %12

licm-latch:                                       ; preds = %9
  br label %licm-header

14:                                               ; preds = %9
  ret i32 2
}
