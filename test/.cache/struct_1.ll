; ModuleID = 'compile/struct_1.c'
source_filename = "compile/struct_1.c"

define i32 @test1(ptr %0) {
  %2 = getelementptr inbounds { i32, i32 }, ptr %0, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr inbounds { i32, i32 }, ptr %0, i32 0, i32 1
  %5 = load i32, ptr %4, align 4
  %6 = add i32 %3, %5
  ret i32 %6
}

define i32 @main() {
  %t = alloca { i32, i32 }, align 8
  %1 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 0
  store i32 2, ptr %1, align 4
  %2 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 1
  store i32 9, ptr %2, align 4
  %3 = tail call i32 @test1(ptr %t)
  ret i32 %3
}
