; ModuleID = 'compile/ackermann.c'
source_filename = "compile/ackermann.c"

declare i32 @atoi(ptr)

define i32 @ack(i32 %0, i32 %1) {
  %3 = icmp eq i32 %0, 0
  br i1 %3, label %4, label %6

4:                                                ; preds = %2
  %5 = add i32 %1, 1
  ret i32 %5

6:                                                ; preds = %2
  %7 = icmp eq i32 %1, 0
  br i1 %7, label %8, label %11

8:                                                ; preds = %6
  %9 = sub i32 %0, 1
  %10 = tail call i32 @ack(i32 %9, i32 1)
  ret i32 %10

11:                                               ; preds = %6
  %12 = sub i32 %1, 1
  %13 = call i32 @ack(i32 %0, i32 %12)
  %14 = sub i32 %0, 1
  %15 = tail call i32 @ack(i32 %14, i32 %13)
  ret i32 %15
}

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp ne i32 %0, 3
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  ret i32 1

5:                                                ; preds = %2
  %6 = getelementptr inbounds ptr, ptr %1, i32 1
  %7 = load ptr, ptr %6, align 8
  %8 = call i32 @atoi(ptr %7)
  %9 = getelementptr inbounds ptr, ptr %1, i32 2
  %10 = load ptr, ptr %9, align 8
  %11 = call i32 @atoi(ptr %10)
  %12 = tail call i32 @ack(i32 %8, i32 %11)
  ret i32 %12
}
