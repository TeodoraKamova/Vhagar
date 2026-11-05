; ModuleID = 'compile/crater.c'
source_filename = "compile/crater.c"

define i32 @main(i32 %0, i32 %1) {
  %3 = icmp ne i32 %0, 1
  %4 = select i1 %3, i32 1, i32 0
  %5 = icmp ne i32 %4, 0
  br i1 %5, label %6, label %7

6:                                                ; preds = %2
  ret i32 1

7:                                                ; preds = %2
  ret i32 0
}
