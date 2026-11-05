; ModuleID = 'compile/struct_accesses.c'
source_filename = "compile/struct_accesses.c"

define i32 @test_st(ptr %0) {
  %s = alloca { { i32 } }, align 8
  %2 = load { { i32 } }, ptr %0, align 4
  store { { i32 } } %2, ptr %s, align 4
  %3 = getelementptr inbounds { { i32 } }, ptr %0, i32 0, i32 0
  %4 = getelementptr inbounds { i32 }, ptr %3, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = getelementptr inbounds { { i32 } }, ptr %s, i32 0, i32 0
  %7 = getelementptr inbounds { i32 }, ptr %6, i32 0, i32 0
  %8 = load i32, ptr %7, align 4
  %9 = add i32 %5, %8
  %10 = getelementptr inbounds { { i32 } }, ptr %0, i32 0, i32 0
  %11 = getelementptr inbounds { i32 }, ptr %10, i32 0, i32 0
  %12 = load i32, ptr %11, align 4
  %13 = add i32 %9, %12
  %14 = getelementptr inbounds { { i32 } }, ptr %s, i32 0, i32 0
  %15 = getelementptr inbounds { i32 }, ptr %14, i32 0, i32 0
  %16 = load i32, ptr %15, align 4
  %17 = add i32 %13, %16
  ret i32 %17
}

define i32 @test_sp(ptr %0) {
  %s = alloca { ptr }, align 8
  %2 = load { ptr }, ptr %0, align 8
  store { ptr } %2, ptr %s, align 8
  %3 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %4 = load ptr, ptr %3, align 8
  %5 = getelementptr inbounds { i32 }, ptr %4, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  %8 = load ptr, ptr %7, align 8
  %9 = getelementptr inbounds { i32 }, ptr %8, i32 0, i32 0
  %10 = load i32, ptr %9, align 4
  %11 = add i32 %6, %10
  %12 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %13 = load ptr, ptr %12, align 8
  %14 = getelementptr inbounds { i32 }, ptr %13, i32 0, i32 0
  %15 = load i32, ptr %14, align 4
  %16 = add i32 %11, %15
  %17 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %18 = load ptr, ptr %17, align 8
  %19 = getelementptr inbounds { i32 }, ptr %18, i32 0, i32 0
  %20 = load i32, ptr %19, align 4
  %21 = add i32 %16, %20
  %22 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  %23 = load ptr, ptr %22, align 8
  %24 = getelementptr inbounds { i32 }, ptr %23, i32 0, i32 0
  %25 = load i32, ptr %24, align 4
  %26 = add i32 %21, %25
  ret i32 %26
}

define i32 @main() {
  %st = alloca { { i32 } }, align 8
  %sp = alloca { ptr }, align 8
  %1 = getelementptr inbounds { { i32 } }, ptr %st, i32 0, i32 0
  %2 = getelementptr inbounds { i32 }, ptr %1, i32 0, i32 0
  store i32 11, ptr %2, align 4
  %3 = getelementptr inbounds { ptr }, ptr %sp, i32 0, i32 0
  %4 = getelementptr inbounds { { i32 } }, ptr %st, i32 0, i32 0
  store ptr %4, ptr %3, align 8
  %5 = call i32 @test_st(ptr %st)
  %6 = icmp ne i32 %5, 44
  br i1 %6, label %7, label %8

7:                                                ; preds = %0
  ret i32 1

8:                                                ; preds = %0
  %9 = call i32 @test_sp(ptr %sp)
  %10 = icmp ne i32 %9, 55
  br i1 %10, label %11, label %12

11:                                               ; preds = %8
  ret i32 2

12:                                               ; preds = %8
  ret i32 0
}
