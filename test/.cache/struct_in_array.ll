; ModuleID = 'compile/struct_in_array.c'
source_filename = "compile/struct_in_array.c"

declare ptr @c4_malloc(i32)

declare void @c4_free(ptr)

define i32 @test_arr(ptr %0, i32 %1) {
  %3 = getelementptr inbounds { i32 }, ptr %0, i32 %1
  %4 = getelementptr inbounds { i32 }, ptr %3, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = getelementptr inbounds { i32 }, ptr %0, i32 %1
  %7 = getelementptr inbounds { i32 }, ptr %6, i32 0, i32 0
  %8 = load i32, ptr %7, align 4
  %9 = add i32 %5, %8
  %10 = getelementptr inbounds { i32 }, ptr %0, i32 %1
  %11 = getelementptr inbounds { i32 }, ptr %10, i32 0, i32 0
  %12 = load i32, ptr %11, align 4
  %13 = add i32 %9, %12
  %14 = getelementptr inbounds { i32 }, ptr %0, i32 %1
  %15 = getelementptr inbounds { i32 }, ptr %14, i32 0, i32 0
  %16 = load i32, ptr %15, align 4
  %17 = add i32 %13, %16
  ret i32 %17
}

define i32 @main() {
  %1 = call ptr @malloc(i32 12)
  %2 = getelementptr inbounds { i32 }, ptr %1, i32 0
  %3 = getelementptr inbounds { i32 }, ptr %2, i32 0, i32 0
  store i32 19, ptr %3, align 4
  %4 = getelementptr inbounds { i32 }, ptr %1, i32 1
  %5 = getelementptr inbounds { i32 }, ptr %4, i32 0, i32 0
  store i32 20, ptr %5, align 4
  %6 = getelementptr inbounds { i32 }, ptr %1, i32 2
  %7 = getelementptr inbounds { i32 }, ptr %6, i32 0, i32 0
  store i32 21, ptr %7, align 4
  %8 = call i32 @test_arr(ptr %1, i32 1)
  call void @free(ptr %1)
  %9 = icmp ne i32 %8, 80
  br i1 %9, label %10, label %11

10:                                               ; preds = %0
  ret i32 1

11:                                               ; preds = %0
  ret i32 0
}

declare ptr @malloc(i32)

declare void @free(ptr)
