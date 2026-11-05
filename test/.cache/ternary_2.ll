; ModuleID = 'compile/ternary_2.c'
source_filename = "compile/ternary_2.c"

@0 = private unnamed_addr constant [16 x i8] c"print me maybe!\00", align 1

declare i32 @puts(ptr)

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %7

5:                                                ; preds = %2
  %6 = call i32 @puts(ptr @0)
  br label %7

7:                                                ; preds = %5, %4
  %.0 = phi i32 [ 67, %4 ], [ %6, %5 ]
  ret i32 %.0
}
