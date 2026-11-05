; ModuleID = 'compile/interpreter_2.c'
source_filename = "compile/interpreter_2.c"

declare ptr @c4_malloc(i32)

declare void @c4_free(ptr)

declare i32 @c4_atoi(ptr)

declare i32 @c4_strlen(ptr)

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

define i32 @apply(ptr %0, i32 %1, i32 %2) {
  %4 = tail call i32 %0(i32 %1, i32 %2)
  ret i32 %4
}

define i32 @interpret(ptr %0, i32 %1) {
entry:
  %2 = shl i32 %1, 2
  %3 = call ptr @malloc(i32 %2)
  br label %licm-header

4:                                                ; preds = %licm-header
  %5 = getelementptr inbounds i8, ptr %0, i32 %i.0
  %6 = load i8, ptr %5, align 1
  %7 = call ptr @dispatch(i8 %6)
  %8 = icmp ne ptr %7, null
  br i1 %8, label %14, label %25

9:                                                ; preds = %licm-header
  %10 = sub i32 %sp.0, 1
  %11 = getelementptr inbounds i32, ptr %3, i32 %10
  %12 = load i32, ptr %11, align 4
  call void @free(ptr %3)
  ret i32 %12

licm-header:                                      ; preds = %entry, %licm-latch
  %sp.0 = phi i32 [ %sp.1, %licm-latch ], [ 0, %entry ]
  %i.0 = phi i32 [ %28, %licm-latch ], [ 0, %entry ]
  %13 = icmp slt i32 %i.0, %1
  br i1 %13, label %4, label %9

14:                                               ; preds = %4
  %15 = sub i32 %sp.0, 2
  %16 = getelementptr inbounds i32, ptr %3, i32 %15
  %17 = load i32, ptr %16, align 4
  %18 = sub i32 %sp.0, 1
  %19 = getelementptr inbounds i32, ptr %3, i32 %18
  %20 = load i32, ptr %19, align 4
  %21 = sub i32 %sp.0, 2
  %22 = getelementptr inbounds i32, ptr %3, i32 %21
  %23 = call i32 @apply(ptr %7, i32 %17, i32 %20)
  store i32 %23, ptr %22, align 4
  %24 = sub i32 %sp.0, 1
  br label %licm-latch

25:                                               ; preds = %4
  %26 = sext i8 %6 to i32
  %27 = icmp eq i32 %26, 32
  br i1 %27, label %29, label %30

licm-latch:                                       ; preds = %35, %14
  %sp.1 = phi i32 [ %24, %14 ], [ %sp.2, %35 ]
  %28 = add i32 %i.0, 1
  br label %licm-header

29:                                               ; preds = %25
  br label %35

30:                                               ; preds = %25
  %31 = getelementptr inbounds i32, ptr %3, i32 %sp.0
  %32 = getelementptr inbounds i8, ptr %0, i32 %i.0
  %33 = call i32 @c4_atoi(ptr %32)
  store i32 %33, ptr %31, align 4
  %34 = add i32 %sp.0, 1
  br label %35

35:                                               ; preds = %30, %29
  %sp.2 = phi i32 [ %sp.0, %29 ], [ %34, %30 ]
  br label %licm-latch
}

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  ret i32 1

5:                                                ; preds = %2
  %6 = getelementptr inbounds ptr, ptr %1, i32 1
  %7 = load ptr, ptr %6, align 8
  %8 = getelementptr inbounds ptr, ptr %1, i32 1
  %9 = load ptr, ptr %8, align 8
  %10 = call i32 @c4_strlen(ptr %9)
  %11 = tail call i32 @interpret(ptr %7, i32 %10)
  ret i32 %11
}

declare ptr @malloc(i32)

declare void @free(ptr)
