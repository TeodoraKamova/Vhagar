; ModuleID = 'compile/isqrt.c'
source_filename = "compile/isqrt.c"

define i32 @isqrt(i32 %0) {
entry:
  %1 = icmp slt i32 %0, 0
  br i1 %1, label %2, label %3

2:                                                ; preds = %entry
  ret i32 -1

3:                                                ; preds = %entry
  %4 = lshr i32 %0, 31
  %5 = add i32 %0, %4
  %6 = ashr i32 %5, 5
  %7 = add i32 %6, 8
  %8 = icmp sgt i32 %7, 65535
  br i1 %8, label %9, label %10

9:                                                ; preds = %3
  br label %10

10:                                               ; preds = %9, %3
  %b.0 = phi i32 [ 65535, %9 ], [ %7, %3 ]
  %11 = add i32 1, %b.0
  %12 = lshr i32 %11, 31
  %13 = add i32 %11, %12
  %14 = ashr i32 %13, 1
  %15 = mul i32 %14, %14
  %16 = icmp sgt i32 %15, %0
  br i1 %16, label %17, label %19

17:                                               ; preds = %10
  %18 = sub i32 %14, 1
  br label %21

19:                                               ; preds = %10
  %20 = add i32 %14, 1
  br label %21

21:                                               ; preds = %19, %17
  %b.1 = phi i32 [ %18, %17 ], [ %b.0, %19 ]
  %a.0 = phi i32 [ 1, %17 ], [ %20, %19 ]
  br label %licm-header

22:                                               ; preds = %licm-header
  %23 = add i32 %a.1, %b.2
  %24 = lshr i32 %23, 31
  %25 = add i32 %23, %24
  %26 = ashr i32 %25, 1
  %27 = mul i32 %26, %26
  %28 = icmp sgt i32 %27, %0
  br i1 %28, label %32, label %34

29:                                               ; preds = %licm-header
  %30 = sub i32 %a.1, 1
  ret i32 %30

licm-header:                                      ; preds = %21, %licm-latch
  %b.2 = phi i32 [ %b.3, %licm-latch ], [ %b.1, %21 ]
  %a.1 = phi i32 [ %a.2, %licm-latch ], [ %a.0, %21 ]
  %31 = icmp sge i32 %b.2, %a.1
  br i1 %31, label %22, label %29

32:                                               ; preds = %22
  %33 = sub i32 %26, 1
  br label %licm-latch

34:                                               ; preds = %22
  %35 = add i32 %26, 1
  br label %licm-latch

licm-latch:                                       ; preds = %34, %32
  %b.3 = phi i32 [ %33, %32 ], [ %b.2, %34 ]
  %a.2 = phi i32 [ %a.1, %32 ], [ %35, %34 ]
  br label %licm-header
}

define i32 @main() {
  %1 = call i32 @isqrt(i32 123456)
  %2 = icmp ne i32 %1, 351
  %3 = zext i1 %2 to i32
  ret i32 %3
}
