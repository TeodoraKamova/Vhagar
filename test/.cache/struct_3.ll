; ModuleID = 'compile/struct_3.c'
source_filename = "compile/struct_3.c"

define i32 @test3(ptr %0) {
  %2 = load ptr, ptr %0, align 8
  %3 = getelementptr inbounds { i32, i32 }, ptr %2, i32 0, i32 0
  %4 = load i32, ptr %3, align 4
  %5 = load ptr, ptr %0, align 8
  %6 = getelementptr inbounds { i32, i32 }, ptr %5, i32 0, i32 1
  %7 = load i32, ptr %6, align 4
  %8 = add i32 %4, %7
  ret i32 %8
}

define i32 @main() {
  %t = alloca { i32, i32 }, align 8
  %tp = alloca ptr, align 8
  %1 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 0
  store i32 2, ptr %1, align 4
  %2 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 1
  store i32 9, ptr %2, align 4
  store ptr %t, ptr %tp, align 8
  %3 = tail call i32 @test3(ptr %tp)
  ret i32 %3
}
