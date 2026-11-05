; ModuleID = 'compile/short_circuit_2.c'
source_filename = "compile/short_circuit_2.c"

@0 = private unnamed_addr constant [24 x i8] c"this should be printed!\00", align 1

declare i32 @puts(ptr)

define i32 @main() {
  %1 = call i32 @puts(ptr @0)
  %2 = icmp sge i32 %1, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  br label %4

4:                                                ; preds = %3, %0
  %x.0 = phi i32 [ 98, %3 ], [ 1, %0 ]
  %5 = call i32 @puts(ptr @0)
  %6 = icmp sge i32 %5, 0
  br i1 %6, label %7, label %8

7:                                                ; preds = %4
  br label %8

8:                                                ; preds = %7, %4
  %x.1 = phi i32 [ 99, %7 ], [ %x.0, %4 ]
  ret i32 %x.1
}
