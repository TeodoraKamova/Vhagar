; ModuleID = 'compile/tail_call_sum.c'
source_filename = "compile/tail_call_sum.c"

declare ptr @c4_malloc(i32)

define i32 @sum_helper(ptr %0, i32 %1, i32 %2, i32 %3) {
  %5 = icmp sge i32 %1, %2
  br i1 %5, label %6, label %7

6:                                                ; preds = %4
  ret i32 %3

7:                                                ; preds = %4
  %8 = add i32 %1, 1
  %9 = getelementptr inbounds i32, ptr %0, i32 %1
  %10 = load i32, ptr %9, align 4
  %11 = add i32 %3, %10
  %12 = tail call i32 @sum_helper(ptr %0, i32 %8, i32 %2, i32 %11)
  ret i32 %12
}

define i32 @sum(ptr %0, i32 %1) {
  %3 = tail call i32 @sum_helper(ptr %0, i32 0, i32 %1, i32 0)
  ret i32 %3
}

define i32 @main() {
  %1 = call ptr @malloc(i32 40)
  %2 = getelementptr inbounds i32, ptr %1, i32 0
  store i32 10, ptr %2, align 4
  %3 = getelementptr inbounds i32, ptr %1, i32 1
  store i32 -20, ptr %3, align 4
  %4 = getelementptr inbounds i32, ptr %1, i32 2
  store i32 35, ptr %4, align 4
  %5 = getelementptr inbounds i32, ptr %1, i32 3
  store i32 -50, ptr %5, align 4
  %6 = getelementptr inbounds i32, ptr %1, i32 4
  store i32 75, ptr %6, align 4
  %7 = getelementptr inbounds i32, ptr %1, i32 5
  store i32 -100, ptr %7, align 4
  %8 = getelementptr inbounds i32, ptr %1, i32 6
  store i32 0, ptr %8, align 4
  %9 = getelementptr inbounds i32, ptr %1, i32 7
  store i32 45, ptr %9, align 4
  %10 = getelementptr inbounds i32, ptr %1, i32 8
  store i32 -80, ptr %10, align 4
  %11 = getelementptr inbounds i32, ptr %1, i32 9
  store i32 110, ptr %11, align 4
  %12 = tail call i32 @sum(ptr %1, i32 10)
  ret i32 %12
}

declare ptr @malloc(i32)
