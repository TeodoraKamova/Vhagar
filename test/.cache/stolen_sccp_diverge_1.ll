; ModuleID = 'compile/stolen_sccp_diverge_1.c'
source_filename = "compile/stolen_sccp_diverge_1.c"

define i32 @f() {
  ret i32 100
}

define i32 @main() {
  %1 = call i32 @f()
  %2 = icmp sgt i32 %1, 1000
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 1

4:                                                ; preds = %0
  %5 = call i32 @f()
  %6 = icmp sge i32 %5, 1000
  br i1 %6, label %7, label %8

7:                                                ; preds = %4
  ret i32 1

8:                                                ; preds = %4
  %9 = call i32 @f()
  %10 = icmp slt i32 1000, %9
  br i1 %10, label %11, label %12

11:                                               ; preds = %8
  ret i32 1

12:                                               ; preds = %8
  %13 = call i32 @f()
  %14 = icmp sle i32 1000, %13
  br i1 %14, label %15, label %16

15:                                               ; preds = %12
  ret i32 1

16:                                               ; preds = %12
  %17 = call i32 @f()
  %18 = icmp eq i32 1000, %17
  br i1 %18, label %19, label %20

19:                                               ; preds = %16
  ret i32 1

20:                                               ; preds = %16
  %21 = call i32 @f()
  %22 = icmp ne i32 100, %21
  br i1 %22, label %23, label %24

23:                                               ; preds = %20
  ret i32 1

24:                                               ; preds = %20
  ret i32 0
}
