; ModuleID = 'compile/interpreter.c'
source_filename = "compile/interpreter.c"

define i32 @add(i32 %0, i32 %1) {
  %3 = add i32 %0, %1
  ret i32 %3
}

define i32 @sub(i32 %0, i32 %1) {
  %3 = sub i32 %0, %1
  ret i32 %3
}

define i32 @mul(i32 %0, i32 %1) {
  %3 = mul i32 %0, %1
  ret i32 %3
}

define i32 @div(i32 %0, i32 %1) {
  %3 = icmp eq i32 %1, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %7

5:                                                ; preds = %2
  %6 = sdiv i32 %0, %1
  br label %7

7:                                                ; preds = %5, %4
  %.0 = phi i32 [ 0, %4 ], [ %6, %5 ]
  ret i32 %.0
}

define i32 @apply(ptr %0, i32 %1, i32 %2) {
  %4 = tail call i32 %0(i32 %1, i32 %2)
  ret i32 %4
}

define ptr @dispatch(i8 %0) {
  %2 = sext i8 %0 to i32
  %3 = icmp eq i32 %2, 43
  br i1 %3, label %4, label %5

4:                                                ; preds = %1
  ret ptr @add

5:                                                ; preds = %1
  %6 = sext i8 %0 to i32
  %7 = icmp eq i32 %6, 45
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  ret ptr @sub

9:                                                ; preds = %5
  %10 = sext i8 %0 to i32
  %11 = icmp eq i32 %10, 42
  br i1 %11, label %12, label %13

12:                                               ; preds = %9
  ret ptr @mul

13:                                               ; preds = %9
  %14 = sext i8 %0 to i32
  %15 = icmp eq i32 %14, 47
  br i1 %15, label %16, label %17

16:                                               ; preds = %13
  ret ptr @div

17:                                               ; preds = %13
  ret ptr null
}

define i32 @main() {
  %1 = call ptr @dispatch(i8 43)
  %2 = tail call i32 @apply(ptr %1, i32 9, i32 2)
  ret i32 %2
}
