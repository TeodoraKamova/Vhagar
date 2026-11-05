; ModuleID = 'compile/func_ptr_2.c'
source_filename = "compile/func_ptr_2.c"

define i32 @foo(i32 %0, i32 %1) {
  %3 = icmp eq i32 %0, 2
  br i1 %3, label %7, label %5

4:                                                ; preds = %7
  ret i32 0

5:                                                ; preds = %7, %2
  %6 = add i32 %0, %1
  ret i32 %6

7:                                                ; preds = %2
  %8 = icmp eq i32 %1, 9
  br i1 %8, label %4, label %5
}

define i32 @main() {
  %1 = icmp ne ptr @foo, null
  br i1 %1, label %2, label %4

2:                                                ; preds = %0
  %3 = tail call i32 @foo(i32 2, i32 9)
  ret i32 %3

4:                                                ; preds = %0
  ret i32 1
}
