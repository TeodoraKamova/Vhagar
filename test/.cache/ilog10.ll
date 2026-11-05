; ModuleID = 'compile/ilog10.c'
source_filename = "compile/ilog10.c"

define i32 @ilog10(i32 %0) {
  %2 = icmp sgt i32 %0, 99
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = icmp slt i32 %0, 1000000
  br i1 %4, label %7, label %9

5:                                                ; preds = %1
  %6 = icmp sgt i32 %0, 9
  br i1 %6, label %47, label %48

7:                                                ; preds = %3
  %8 = icmp slt i32 %0, 10000
  br i1 %8, label %11, label %20

9:                                                ; preds = %3
  %10 = icmp slt i32 %0, 100000000
  br i1 %10, label %29, label %38

11:                                               ; preds = %7
  %12 = sub i32 %0, 1000
  %13 = lshr i32 %12, 31
  %14 = add i32 %12, %13
  %15 = ashr i32 %14, 30
  %16 = lshr i32 %15, 31
  %17 = add i32 %15, %16
  %18 = ashr i32 %17, 1
  %19 = add i32 3, %18
  ret i32 %19

20:                                               ; preds = %7
  %21 = sub i32 %0, 10000
  %22 = lshr i32 %21, 31
  %23 = add i32 %21, %22
  %24 = ashr i32 %23, 30
  %25 = lshr i32 %24, 31
  %26 = add i32 %24, %25
  %27 = ashr i32 %26, 1
  %28 = add i32 5, %27
  ret i32 %28

29:                                               ; preds = %9
  %30 = sub i32 %0, 10000000
  %31 = lshr i32 %30, 31
  %32 = add i32 %30, %31
  %33 = ashr i32 %32, 30
  %34 = lshr i32 %33, 31
  %35 = add i32 %33, %34
  %36 = ashr i32 %35, 1
  %37 = add i32 7, %36
  ret i32 %37

38:                                               ; preds = %9
  %39 = sub i32 %0, 1000000000
  %40 = lshr i32 %39, 31
  %41 = add i32 %39, %40
  %42 = ashr i32 %41, 30
  %43 = lshr i32 %42, 31
  %44 = add i32 %42, %43
  %45 = ashr i32 %44, 1
  %46 = add i32 9, %45
  ret i32 %46

47:                                               ; preds = %5
  ret i32 1

48:                                               ; preds = %5
  %49 = sub i32 %0, 1
  %50 = lshr i32 %49, 31
  %51 = add i32 %49, %50
  %52 = ashr i32 %51, 30
  %53 = lshr i32 %52, 31
  %54 = add i32 %52, %53
  %55 = ashr i32 %54, 1
  ret i32 %55
}

define i32 @main() {
  %1 = call i32 @ilog10(i32 1202002)
  %2 = icmp ne i32 %1, 7
  %3 = zext i1 %2 to i32
  ret i32 %3
}
