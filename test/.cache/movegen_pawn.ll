; ModuleID = 'compile/movegen_pawn.c'
source_filename = "compile/movegen_pawn.c"

define i32 @WE_add8(i32 %0) {
  %2 = and i32 %0, -8
  %3 = shl i32 65280, %2
  ret i32 %3
}

define i32 @ANDNOT(i32 %0, i32 %1) {
  %3 = xor i32 %0, -1
  %4 = and i32 %3, %1
  ret i32 %4
}

define i32 @PawnWhite(i32 %0, i32 %1, i32 %2, i32 %3) {
  %5 = or i32 %0, %1
  %6 = shl i32 256, %3
  %7 = icmp slt i32 %3, 16
  br i1 %7, label %23, label %9

8:                                                ; preds = %23
  br label %10

9:                                                ; preds = %23, %4
  br label %10

10:                                               ; preds = %9, %8
  %.01 = phi i32 [ 0, %9 ], [ 1, %8 ]
  %11 = icmp ne i32 %.01, 0
  %12 = select i1 %11, i32 %6, i32 0
  %13 = shl i32 %12, 8
  %14 = shl i32 640, %3
  %15 = call i32 @WE_add8(i32 %3)
  %16 = and i32 %14, %15
  %17 = and i32 %16, %2
  %18 = and i32 %16, %1
  %19 = or i32 %18, %17
  %20 = or i32 %6, %13
  %21 = call i32 @ANDNOT(i32 %5, i32 %20)
  %22 = or i32 %19, %21
  ret i32 %22

23:                                               ; preds = %4
  %24 = and i32 %5, %6
  %25 = icmp ne i32 %24, 0
  br i1 %25, label %9, label %8
}

define i32 @main() {
  %1 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 8)
  %2 = icmp ne i32 %1, 16842752
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 1

4:                                                ; preds = %0
  %5 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 9)
  %6 = icmp ne i32 %5, 33685504
  br i1 %6, label %7, label %8

7:                                                ; preds = %4
  ret i32 2

8:                                                ; preds = %4
  %9 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 10)
  %10 = icmp ne i32 %9, 67371008
  br i1 %10, label %11, label %12

11:                                               ; preds = %8
  ret i32 3

12:                                               ; preds = %8
  %13 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 11)
  %14 = icmp ne i32 %13, 134742016
  br i1 %14, label %15, label %16

15:                                               ; preds = %12
  ret i32 4

16:                                               ; preds = %12
  %17 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 12)
  %18 = icmp ne i32 %17, 269484032
  br i1 %18, label %19, label %20

19:                                               ; preds = %16
  ret i32 5

20:                                               ; preds = %16
  %21 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 13)
  %22 = icmp ne i32 %21, 538968064
  br i1 %22, label %23, label %24

23:                                               ; preds = %20
  ret i32 6

24:                                               ; preds = %20
  %25 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 14)
  %26 = icmp ne i32 %25, 1077936128
  br i1 %26, label %27, label %28

27:                                               ; preds = %24
  ret i32 7

28:                                               ; preds = %24
  %29 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 15)
  %30 = icmp ne i32 %29, -2139095040
  br i1 %30, label %31, label %32

31:                                               ; preds = %28
  ret i32 8

32:                                               ; preds = %28
  %33 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 16)
  %34 = icmp ne i32 %33, 16777216
  br i1 %34, label %35, label %36

35:                                               ; preds = %32
  ret i32 9

36:                                               ; preds = %32
  %37 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 17)
  %38 = icmp ne i32 %37, 33554432
  br i1 %38, label %39, label %40

39:                                               ; preds = %36
  ret i32 10

40:                                               ; preds = %36
  %41 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 18)
  %42 = icmp ne i32 %41, 67108864
  br i1 %42, label %43, label %44

43:                                               ; preds = %40
  ret i32 11

44:                                               ; preds = %40
  %45 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 19)
  %46 = icmp ne i32 %45, 134217728
  br i1 %46, label %47, label %48

47:                                               ; preds = %44
  ret i32 12

48:                                               ; preds = %44
  %49 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 20)
  %50 = icmp ne i32 %49, 268435456
  br i1 %50, label %51, label %52

51:                                               ; preds = %48
  ret i32 13

52:                                               ; preds = %48
  %53 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 21)
  %54 = icmp ne i32 %53, 536870912
  br i1 %54, label %55, label %56

55:                                               ; preds = %52
  ret i32 14

56:                                               ; preds = %52
  %57 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 22)
  %58 = icmp ne i32 %57, 1073741824
  br i1 %58, label %59, label %60

59:                                               ; preds = %56
  ret i32 15

60:                                               ; preds = %56
  %61 = call i32 @PawnWhite(i32 1310720, i32 939524096, i32 0, i32 18)
  %62 = icmp ne i32 %61, 201326592
  br i1 %62, label %63, label %64

63:                                               ; preds = %60
  ret i32 16

64:                                               ; preds = %60
  %65 = call i32 @PawnWhite(i32 1310720, i32 939524096, i32 0, i32 20)
  %66 = icmp ne i32 %65, 671088640
  br i1 %66, label %67, label %68

67:                                               ; preds = %64
  ret i32 17

68:                                               ; preds = %64
  %69 = call i32 @PawnWhite(i32 65280, i32 0, i32 0, i32 24)
  %70 = icmp ne i32 %69, 0
  br i1 %70, label %71, label %72

71:                                               ; preds = %68
  ret i32 18

72:                                               ; preds = %68
  ret i32 0
}
