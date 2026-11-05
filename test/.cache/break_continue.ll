; ModuleID = 'compile/break_continue.c'
source_filename = "compile/break_continue.c"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare i32 @c4_atoi(ptr)

declare void @c4_print_int(i32)

define i32 @main(i32 %0, ptr %1) {
entry:
  %2 = icmp eq i32 %0, 1
  br i1 %2, label %3, label %4

3:                                                ; preds = %entry
  ret i32 1

4:                                                ; preds = %entry
  %5 = getelementptr inbounds ptr, ptr %1, i32 1
  %6 = load ptr, ptr %5, align 8
  %7 = call i32 @c4_atoi(ptr %6)
  br label %licm-header5

8:                                                ; preds = %licm-header5
  %9 = icmp eq i32 %i.0, 5
  br i1 %9, label %12, label %licm-latch6

10:                                               ; preds = %12, %licm-header5
  br label %licm-header

licm-header5:                                     ; preds = %4, %licm-latch6
  %i.0 = phi i32 [ %14, %licm-latch6 ], [ 0, %4 ]
  %11 = icmp slt i32 %i.0, %7
  br i1 %11, label %8, label %10

12:                                               ; preds = %8
  br label %10

licm-latch6:                                      ; preds = %8
  %13 = call i32 (ptr, ...) @printf(ptr @0, i32 %i.0)
  %14 = add i32 %i.0, 1
  br label %licm-header5

15:                                               ; preds = %licm-header
  %16 = lshr i32 %22, 31
  %17 = add i32 %22, %16
  %18 = and i32 %17, -2
  %19 = sub i32 %22, %18
  %20 = icmp eq i32 %19, 0
  br i1 %20, label %licm-latch, label %licm-latch3

21:                                               ; preds = %licm-header
  ret i32 0

licm-header:                                      ; preds = %10, %licm-latch3, %licm-latch
  %i.1 = phi i32 [ %22, %licm-latch ], [ %22, %licm-latch3 ], [ -1, %10 ]
  %22 = add i32 %i.1, 1
  %23 = icmp slt i32 %22, %7
  br i1 %23, label %15, label %21

licm-latch:                                       ; preds = %15
  br label %licm-header

licm-latch3:                                      ; preds = %15
  %24 = call i32 (ptr, ...) @printf(ptr @0, i32 %22)
  br label %licm-header
}

declare i32 @printf(ptr, ...)
