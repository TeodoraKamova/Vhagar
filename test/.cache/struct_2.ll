; ModuleID = 'compile/struct_2.c'
source_filename = "compile/struct_2.c"

define i32 @test2(ptr %0) {
  %2 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  %4 = getelementptr inbounds { i32, i32 }, ptr %3, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = getelementptr inbounds { ptr }, ptr %0, i32 0, i32 0
  %7 = load ptr, ptr %6, align 8
  %8 = getelementptr inbounds { i32, i32 }, ptr %7, i32 0, i32 1
  %9 = load i32, ptr %8, align 4
  %10 = add i32 %5, %9
  ret i32 %10
}

define i32 @main() {
  %t = alloca { i32, i32 }, align 8
  %s = alloca { ptr }, align 8
  %1 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 0
  store i32 2, ptr %1, align 4
  %2 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 1
  store i32 9, ptr %2, align 4
  %3 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  store ptr %t, ptr %3, align 8
  %4 = tail call i32 @test2(ptr %s)
  ret i32 %4
}
