; ModuleID = 'compile/fact.c'
source_filename = "compile/fact.c"

define i32 @factorial_recursive(i32 %0) {
  %2 = icmp eq i32 %0, 0
  br i1 %2, label %3, label %8

3:                                                ; preds = %8, %1
  ret i32 1

4:                                                ; preds = %8
  %5 = sub i32 %0, 1
  %6 = call i32 @factorial_recursive(i32 %5)
  %7 = mul i32 %0, %6
  ret i32 %7

8:                                                ; preds = %1
  %9 = icmp eq i32 %0, 1
  br i1 %9, label %3, label %4
}

define i32 @factorial_iterative(i32 %0) {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %1 = mul i32 %result.0, %i.0
  %2 = add i32 %i.0, 1
  br label %licm-header

3:                                                ; preds = %licm-header
  ret i32 %result.0

licm-header:                                      ; preds = %entry, %licm-latch
  %result.0 = phi i32 [ %1, %licm-latch ], [ 1, %entry ]
  %i.0 = phi i32 [ %2, %licm-latch ], [ 2, %entry ]
  %4 = icmp sle i32 %i.0, %0
  br i1 %4, label %licm-latch, label %3
}

define i32 @factorial_recursive_ptr(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 0
  br i1 %3, label %4, label %9

4:                                                ; preds = %9, %2
  ret i32 1

5:                                                ; preds = %9
  %6 = sub i32 %0, 1
  %7 = call i32 %1(i32 %6, ptr %1)
  %8 = mul i32 %0, %7
  ret i32 %8

9:                                                ; preds = %2
  %10 = icmp eq i32 %0, 1
  br i1 %10, label %4, label %5
}

define i32 @main() {
  %1 = call i32 @factorial_iterative(i32 9)
  %2 = icmp ne i32 %1, 362880
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 1

4:                                                ; preds = %0
  %5 = call i32 @factorial_recursive(i32 9)
  %6 = icmp ne i32 %5, %1
  br i1 %6, label %7, label %8

7:                                                ; preds = %4
  ret i32 2

8:                                                ; preds = %4
  %9 = call i32 @factorial_recursive_ptr(i32 9, ptr @factorial_recursive_ptr)
  %10 = icmp ne i32 %9, %1
  br i1 %10, label %11, label %12

11:                                               ; preds = %8
  ret i32 3

12:                                               ; preds = %8
  %13 = call i32 @factorial_recursive_ptr(i32 9, ptr @factorial_recursive_ptr)
  %14 = icmp ne i32 %13, %1
  br i1 %14, label %15, label %16

15:                                               ; preds = %12
  ret i32 4

16:                                               ; preds = %12
  ret i32 0
}
