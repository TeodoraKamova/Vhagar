; ModuleID = 'compile/stolen_sccp_diverge_2.c'
source_filename = "compile/stolen_sccp_diverge_2.c"

@g = common global i32 0, align 4

define i32 @effect() {
  store i32 1, ptr @g, align 4
  ret i32 1
}

define i32 @main() {
  store i32 0, ptr @g, align 4
  %1 = load i32, ptr @g, align 4
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 2

4:                                                ; preds = %0
  %5 = call i32 @effect()
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %7, label %10

7:                                                ; preds = %4
  %8 = load i32, ptr @g, align 4
  %9 = icmp ne i32 %8, 1
  br i1 %9, label %11, label %12

10:                                               ; preds = %4
  ret i32 4

11:                                               ; preds = %7
  ret i32 3

12:                                               ; preds = %7
  store i32 0, ptr @g, align 4
  %13 = load i32, ptr @g, align 4
  %14 = icmp ne i32 %13, 0
  br i1 %14, label %15, label %16

15:                                               ; preds = %12
  ret i32 5

16:                                               ; preds = %12
  %17 = call i32 @effect()
  %18 = icmp ne i32 %17, 0
  br i1 %18, label %19, label %22

19:                                               ; preds = %16
  %20 = load i32, ptr @g, align 4
  %21 = icmp ne i32 %20, 1
  br i1 %21, label %23, label %24

22:                                               ; preds = %16
  ret i32 8

23:                                               ; preds = %19
  ret i32 7

24:                                               ; preds = %19
  ret i32 0
}
