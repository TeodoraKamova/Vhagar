; ModuleID = 'compile/tail_call_fib.c'
source_filename = "compile/tail_call_fib.c"

define i32 @fib_helper(i32 %0, i32 %1, i32 %2) {
  %4 = icmp eq i32 %0, 0
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  ret i32 %1

6:                                                ; preds = %3
  %7 = icmp eq i32 %0, 1
  br i1 %7, label %8, label %9

8:                                                ; preds = %6
  ret i32 %2

9:                                                ; preds = %6
  %10 = sub i32 %0, 1
  %11 = add i32 %1, %2
  %12 = tail call i32 @fib_helper(i32 %10, i32 %2, i32 %11)
  ret i32 %12
}

define i32 @fib(i32 %0) {
  %2 = tail call i32 @fib_helper(i32 %0, i32 0, i32 1)
  ret i32 %2
}

define i32 @main() {
  %1 = tail call i32 @fib(i32 40)
  ret i32 %1
}
