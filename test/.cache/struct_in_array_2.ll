; ModuleID = 'compile/struct_in_array_2.c'
source_filename = "compile/struct_in_array_2.c"

declare ptr @c4_malloc(i32)

declare void @c4_free(ptr)

define i32 @test_arr(ptr %0, i32 %1) {
  %3 = getelementptr inbounds ptr, ptr %0, i32 %1
  %4 = load ptr, ptr %3, align 8
  %5 = getelementptr inbounds { i32 }, ptr %4, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = getelementptr inbounds ptr, ptr %0, i32 %1
  %8 = load ptr, ptr %7, align 8
  %9 = getelementptr inbounds { i32 }, ptr %8, i32 0, i32 0
  %10 = load i32, ptr %9, align 4
  %11 = add i32 %6, %10
  %12 = getelementptr inbounds ptr, ptr %0, i32 %1
  %13 = load ptr, ptr %12, align 8
  %14 = getelementptr inbounds { i32 }, ptr %13, i32 0, i32 0
  %15 = load i32, ptr %14, align 4
  %16 = add i32 %11, %15
  ret i32 %16
}

define i32 @main() {
  %s0 = alloca { i32 }, align 8
  %s1 = alloca { i32 }, align 8
  %s2 = alloca { i32 }, align 8
  %1 = call ptr @malloc(i32 24)
  %2 = getelementptr inbounds { i32 }, ptr %s0, i32 0, i32 0
  store i32 19, ptr %2, align 4
  %3 = getelementptr inbounds { i32 }, ptr %s1, i32 0, i32 0
  store i32 20, ptr %3, align 4
  %4 = getelementptr inbounds { i32 }, ptr %s2, i32 0, i32 0
  store i32 21, ptr %4, align 4
  %5 = getelementptr inbounds ptr, ptr %1, i32 0
  store ptr %s0, ptr %5, align 8
  %6 = getelementptr inbounds ptr, ptr %1, i32 1
  store ptr %s1, ptr %6, align 8
  %7 = getelementptr inbounds ptr, ptr %1, i32 2
  store ptr %s2, ptr %7, align 8
  %8 = call i32 @test_arr(ptr %1, i32 1)
  call void @free(ptr %1)
  %9 = icmp ne i32 %8, 60
  br i1 %9, label %10, label %11

10:                                               ; preds = %0
  ret i32 1

11:                                               ; preds = %0
  ret i32 0
}

declare ptr @malloc(i32)

declare void @free(ptr)
