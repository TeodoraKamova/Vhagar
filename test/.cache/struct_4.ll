; ModuleID = 'compile/struct_4.c'
source_filename = "compile/struct_4.c"

define i32 @test2(ptr %0) {
  %2 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds { i32, i32 }, ptr %3, i32 0, i32 0
  store i32 2, ptr %4, align 4
  %5 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %6 = load ptr, ptr %5, align 8
  %7 = getelementptr inbounds { i32, i32 }, ptr %6, i32 0, i32 1
  store i32 9, ptr %7, align 4
  %8 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %9 = load ptr, ptr %8, align 8
  %10 = getelementptr inbounds { i32, i32 }, ptr %9, i32 0, i32 0
  %11 = load i32, ptr %10, align 4
  %12 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %13 = load ptr, ptr %12, align 8
  %14 = getelementptr inbounds { i32, i32 }, ptr %13, i32 0, i32 1
  %15 = load i32, ptr %14, align 4
  %16 = add i32 %11, %15
  ret i32 %16
}

define i32 @main() {
  %t = alloca { i32, i32 }, align 8
  %s = alloca { ptr }, align 8
  %1 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  store ptr %t, ptr %1, align 8
  %2 = tail call i32 @test2(ptr %s)
  ret i32 %2
}
