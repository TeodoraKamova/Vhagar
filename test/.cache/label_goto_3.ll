; ModuleID = 'compile/label_goto_3.c'
source_filename = "compile/label_goto_3.c"

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %6

5:                                                ; preds = %2
  br label %6

6:                                                ; preds = %4, %5
  ret i32 99
}
