; ModuleID = 'compile/const_integer_div.c'
source_filename = "compile/const_integer_div.c"

define i32 @abs(i32 %0) {
  %2 = icmp slt i32 %0, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = sub i32 0, %0
  br label %6

5:                                                ; preds = %1
  br label %6

6:                                                ; preds = %5, %3
  %.0 = phi i32 [ %4, %3 ], [ %0, %5 ]
  ret i32 %.0
}

define { i32, i32, i32 } @magicu(i32 %0) {
entry:
  %magu = alloca { i32, i32, i32 }, align 8
  %1 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 1
  store i32 0, ptr %1, align 4
  %2 = sdiv i32 2147483647, %0
  %3 = mul i32 %2, %0
  %4 = sub i32 2147483647, %3
  %5 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 1
  %6 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 1
  %7 = sub i32 %0, 1
  br label %licm-header

licm-header:                                      ; preds = %entry, %licm-latch
  %q.0 = phi i32 [ %q.1, %licm-latch ], [ %2, %entry ]
  %p32.0 = phi i32 [ %p32.1, %licm-latch ], [ undef, %entry ]
  %p.0 = phi i32 [ %8, %licm-latch ], [ 31, %entry ]
  %r.0 = phi i32 [ %r.1, %licm-latch ], [ %4, %entry ]
  %8 = add i32 %p.0, 1
  %9 = icmp eq i32 %8, 32
  br i1 %9, label %18, label %19

licm-latch:                                       ; preds = %16
  br label %licm-header

10:                                               ; preds = %16, %29
  %11 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 0
  %12 = add i32 %q.1, 1
  store i32 %12, ptr %11, align 4
  %13 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 2
  %14 = sub i32 %8, 32
  store i32 %14, ptr %13, align 4
  %15 = load { i32, i32, i32 }, ptr %magu, align 4
  ret { i32, i32, i32 } %15

16:                                               ; preds = %29
  %17 = icmp slt i32 %p32.1, %30
  br i1 %17, label %licm-latch, label %10

18:                                               ; preds = %licm-header
  br label %21

19:                                               ; preds = %licm-header
  %20 = shl i32 %p32.0, 1
  br label %21

21:                                               ; preds = %19, %18
  %p32.1 = phi i32 [ 1, %18 ], [ %20, %19 ]
  %22 = add i32 %r.0, 1
  %23 = sub i32 %0, %r.0
  %24 = icmp sge i32 %22, %23
  br i1 %24, label %25, label %27

25:                                               ; preds = %21
  %26 = icmp sge i32 %q.0, 2147483647
  br i1 %26, label %32, label %33

27:                                               ; preds = %21
  %28 = icmp sge i32 %q.0, -2147483648
  br i1 %28, label %39, label %40

29:                                               ; preds = %40, %33
  %q.1 = phi i32 [ %35, %33 ], [ %41, %40 ]
  %r.1 = phi i32 [ %38, %33 ], [ %43, %40 ]
  %30 = sub i32 %7, %r.1
  %31 = icmp slt i32 %8, 64
  br i1 %31, label %16, label %10

32:                                               ; preds = %25
  store i32 1, ptr %6, align 4
  br label %33

33:                                               ; preds = %32, %25
  %34 = shl i32 %q.0, 1
  %35 = add i32 %34, 1
  %36 = shl i32 %r.0, 1
  %37 = add i32 %36, 1
  %38 = sub i32 %37, %0
  br label %29

39:                                               ; preds = %27
  store i32 1, ptr %5, align 4
  br label %40

40:                                               ; preds = %39, %27
  %41 = shl i32 %q.0, 1
  %42 = shl i32 %r.0, 1
  %43 = add i32 %42, 1
  br label %29
}

define { i32, i32, i32 } @magicu2(i32 %0) {
entry:
  %magu = alloca { i32, i32, i32 }, align 8
  %1 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 1
  store i32 0, ptr %1, align 4
  %2 = sdiv i32 2147483647, %0
  %3 = mul i32 %2, %0
  %4 = sub i32 2147483647, %3
  %5 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 1
  %6 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 1
  %7 = sub i32 %0, 1
  br label %licm-header

licm-header:                                      ; preds = %entry, %licm-latch
  %q.0 = phi i32 [ %q.1, %licm-latch ], [ %2, %entry ]
  %p32.0 = phi i32 [ %p32.1, %licm-latch ], [ undef, %entry ]
  %p.0 = phi i32 [ %8, %licm-latch ], [ 31, %entry ]
  %r.0 = phi i32 [ %r.1, %licm-latch ], [ %4, %entry ]
  %8 = add i32 %p.0, 1
  %9 = icmp eq i32 %8, 32
  br i1 %9, label %18, label %19

licm-latch:                                       ; preds = %16
  br label %licm-header

10:                                               ; preds = %16, %29
  %11 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 0
  %12 = add i32 %q.1, 1
  store i32 %12, ptr %11, align 4
  %13 = getelementptr inbounds { i32, i32, i32 }, ptr %magu, i32 0, i32 2
  %14 = sub i32 %8, 32
  store i32 %14, ptr %13, align 4
  %15 = load { i32, i32, i32 }, ptr %magu, align 4
  ret { i32, i32, i32 } %15

16:                                               ; preds = %29
  %17 = icmp slt i32 %p32.1, %30
  br i1 %17, label %licm-latch, label %10

18:                                               ; preds = %licm-header
  br label %21

19:                                               ; preds = %licm-header
  %20 = shl i32 %p32.0, 1
  br label %21

21:                                               ; preds = %19, %18
  %p32.1 = phi i32 [ 1, %18 ], [ %20, %19 ]
  %22 = add i32 %r.0, 1
  %23 = sub i32 %0, %r.0
  %24 = icmp sge i32 %22, %23
  br i1 %24, label %25, label %27

25:                                               ; preds = %21
  %26 = icmp sge i32 %q.0, 2147483647
  br i1 %26, label %32, label %33

27:                                               ; preds = %21
  %28 = icmp sge i32 %q.0, -2147483648
  br i1 %28, label %39, label %40

29:                                               ; preds = %40, %33
  %q.1 = phi i32 [ %35, %33 ], [ %41, %40 ]
  %r.1 = phi i32 [ %38, %33 ], [ %43, %40 ]
  %30 = sub i32 %7, %r.1
  %31 = icmp slt i32 %8, 64
  br i1 %31, label %16, label %10

32:                                               ; preds = %25
  store i32 1, ptr %6, align 4
  br label %33

33:                                               ; preds = %32, %25
  %34 = shl i32 %q.0, 1
  %35 = add i32 %34, 1
  %36 = shl i32 %r.0, 1
  %37 = add i32 %36, 1
  %38 = sub i32 %37, %0
  br label %29

39:                                               ; preds = %27
  store i32 1, ptr %5, align 4
  br label %40

40:                                               ; preds = %39, %27
  %41 = shl i32 %q.0, 1
  %42 = shl i32 %r.0, 1
  %43 = add i32 %42, 1
  br label %29
}

define { i32, i32 } @magic(i32 %0) {
entry:
  %mag = alloca { i32, i32 }, align 8
  %1 = call i32 @abs(i32 %0)
  %2 = lshr i32 %0, 31
  %3 = add i32 -2147483648, %2
  %4 = sub i32 %3, 1
  %5 = srem i32 %3, %1
  %6 = sub i32 %4, %5
  %7 = sdiv i32 -2147483648, %6
  %8 = mul i32 %7, %6
  %9 = sub i32 -2147483648, %8
  %10 = sdiv i32 -2147483648, %1
  %11 = mul i32 %10, %1
  %12 = sub i32 -2147483648, %11
  br label %licm-header

licm-header:                                      ; preds = %entry, %licm-latch
  %q1.0 = phi i32 [ %q1.1, %licm-latch ], [ %7, %entry ]
  %r1.0 = phi i32 [ %r1.1, %licm-latch ], [ %9, %entry ]
  %q2.0 = phi i32 [ %q2.1, %licm-latch ], [ %10, %entry ]
  %p.0 = phi i32 [ %13, %licm-latch ], [ 31, %entry ]
  %r2.0 = phi i32 [ %r2.1, %licm-latch ], [ %12, %entry ]
  %13 = add i32 %p.0, 1
  %14 = shl i32 %q1.0, 1
  %15 = shl i32 %r1.0, 1
  %16 = icmp sge i32 %15, %6
  br i1 %16, label %25, label %28

licm-latch:                                       ; preds = %23, %35
  br label %licm-header

17:                                               ; preds = %23, %21
  %18 = getelementptr inbounds { i32, i32 }, ptr %mag, i32 0, i32 0
  %19 = add i32 %q2.1, 1
  store i32 %19, ptr %18, align 4
  %20 = icmp slt i32 %0, 0
  br i1 %20, label %38, label %43

21:                                               ; preds = %35
  %22 = icmp eq i32 %q1.1, %36
  br i1 %22, label %23, label %17

23:                                               ; preds = %21
  %24 = icmp eq i32 %r1.1, 0
  br i1 %24, label %licm-latch, label %17

25:                                               ; preds = %licm-header
  %26 = add i32 %14, 1
  %27 = sub i32 %15, %6
  br label %28

28:                                               ; preds = %25, %licm-header
  %q1.1 = phi i32 [ %26, %25 ], [ %14, %licm-header ]
  %r1.1 = phi i32 [ %27, %25 ], [ %15, %licm-header ]
  %29 = shl i32 %q2.0, 1
  %30 = shl i32 %r2.0, 1
  %31 = icmp sge i32 %30, %1
  br i1 %31, label %32, label %35

32:                                               ; preds = %28
  %33 = add i32 %29, 1
  %34 = sub i32 %30, %1
  br label %35

35:                                               ; preds = %32, %28
  %q2.1 = phi i32 [ %33, %32 ], [ %29, %28 ]
  %r2.1 = phi i32 [ %34, %32 ], [ %30, %28 ]
  %36 = sub i32 %1, %r2.1
  %37 = icmp slt i32 %q1.1, %36
  br i1 %37, label %licm-latch, label %21

38:                                               ; preds = %17
  %39 = getelementptr inbounds { i32, i32 }, ptr %mag, i32 0, i32 0
  %40 = getelementptr inbounds { i32, i32 }, ptr %mag, i32 0, i32 0
  %41 = load i32, ptr %40, align 4
  %42 = sub i32 0, %41
  store i32 %42, ptr %39, align 4
  br label %43

43:                                               ; preds = %38, %17
  %44 = getelementptr inbounds { i32, i32 }, ptr %mag, i32 0, i32 1
  %45 = sub i32 %13, 32
  store i32 %45, ptr %44, align 4
  %46 = load { i32, i32 }, ptr %mag, align 4
  ret { i32, i32 } %46
}

define i32 @main() {
  %uns1 = alloca { i32, i32, i32 }, align 8
  %uns2 = alloca { i32, i32, i32 }, align 8
  %sig = alloca { i32, i32 }, align 8
  %1 = call { i32, i32, i32 } @magicu(i32 50)
  store { i32, i32, i32 } %1, ptr %uns1, align 4
  %2 = call { i32, i32, i32 } @magicu2(i32 50)
  store { i32, i32, i32 } %2, ptr %uns2, align 4
  %3 = getelementptr inbounds { i32, i32, i32 }, ptr %uns1, i32 0, i32 1
  %4 = load i32, ptr %3, align 4
  %5 = getelementptr inbounds { i32, i32, i32 }, ptr %uns2, i32 0, i32 1
  %6 = load i32, ptr %5, align 4
  %7 = icmp ne i32 %4, %6
  br i1 %7, label %8, label %9

8:                                                ; preds = %0
  ret i32 1

9:                                                ; preds = %0
  %10 = getelementptr inbounds { i32, i32, i32 }, ptr %uns1, i32 0, i32 2
  %11 = load i32, ptr %10, align 4
  %12 = getelementptr inbounds { i32, i32, i32 }, ptr %uns2, i32 0, i32 2
  %13 = load i32, ptr %12, align 4
  %14 = icmp ne i32 %11, %13
  br i1 %14, label %15, label %16

15:                                               ; preds = %9
  ret i32 2

16:                                               ; preds = %9
  %17 = getelementptr inbounds { i32, i32, i32 }, ptr %uns1, i32 0, i32 0
  %18 = load i32, ptr %17, align 4
  %19 = getelementptr inbounds { i32, i32, i32 }, ptr %uns2, i32 0, i32 0
  %20 = load i32, ptr %19, align 4
  %21 = icmp ne i32 %18, %20
  br i1 %21, label %22, label %23

22:                                               ; preds = %16
  ret i32 3

23:                                               ; preds = %16
  %24 = call { i32, i32 } @magic(i32 -15)
  store { i32, i32 } %24, ptr %sig, align 4
  %25 = getelementptr inbounds { i32, i32 }, ptr %sig, i32 0, i32 1
  %26 = load i32, ptr %25, align 4
  %27 = icmp ne i32 %26, 27
  br i1 %27, label %28, label %29

28:                                               ; preds = %23
  ret i32 4

29:                                               ; preds = %23
  %30 = getelementptr inbounds { i32, i32 }, ptr %sig, i32 0, i32 0
  %31 = load i32, ptr %30, align 4
  %32 = icmp ne i32 %31, 2147483647
  br i1 %32, label %33, label %34

33:                                               ; preds = %29
  ret i32 5

34:                                               ; preds = %29
  ret i32 0
}
