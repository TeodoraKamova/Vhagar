; ModuleID = 'compile/const_int_div_3.c'
source_filename = "compile/const_int_div_3.c"

@0 = private unnamed_addr constant [4 x i8] c"%d \00", align 1

declare i32 @c4_atoi(ptr)

declare void @c4_print_int(i32)

declare void @c4_print_str(ptr)

define i32 @foo(i32 %0) {
  %2 = sext i32 %0 to i64
  %3 = mul i64 %2, 1321528399
  %4 = lshr i64 %3, 32
  %5 = trunc i64 %4 to i32
  %6 = ashr i32 %5, 2
  %7 = lshr i32 %6, 31
  %8 = add i32 %6, %7
  ret i32 %8
}

define i32 @main(i32 %0, ptr %1) {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %2 = getelementptr inbounds ptr, ptr %1, i32 %i.0
  %3 = load ptr, ptr %2, align 8
  %4 = call i32 @c4_atoi(ptr %3)
  %5 = call i32 @foo(i32 %4)
  %6 = call i32 (ptr, ...) @printf(ptr @0, i32 %5)
  %7 = add i32 %i.0, 1
  br label %licm-header

8:                                                ; preds = %licm-header
  ret i32 0

licm-header:                                      ; preds = %entry, %licm-latch
  %i.0 = phi i32 [ %7, %licm-latch ], [ 1, %entry ]
  %9 = icmp slt i32 %i.0, %0
  br i1 %9, label %licm-latch, label %8
}

declare i32 @printf(ptr, ...)
