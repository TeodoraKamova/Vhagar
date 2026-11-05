; ModuleID = 'compile/short_circuit_3.c'
source_filename = "compile/short_circuit_3.c"

@0 = private unnamed_addr constant [13 x i8] c"print me! #1\00", align 1
@1 = private unnamed_addr constant [13 x i8] c"print me! #2\00", align 1

declare i32 @puts(ptr)

define i32 @main() {
  %1 = call i32 @puts(ptr @0)
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  br label %5

4:                                                ; preds = %0
  br label %5

5:                                                ; preds = %4, %3
  %6 = call i32 @puts(ptr @1)
  %7 = icmp ne i32 %6, 0
  br i1 %7, label %8, label %9

8:                                                ; preds = %5
  br label %10

9:                                                ; preds = %5
  br label %10

10:                                               ; preds = %9, %8
  ret i32 99
}
