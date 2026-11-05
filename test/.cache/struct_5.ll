; ModuleID = 'compile/struct_5.c'
source_filename = "compile/struct_5.c"

define ptr @id(ptr %0) {
  ret ptr %0
}

define i32 @main() {
  %t = alloca { i32, i32 }, align 8
  %1 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 0
  store i32 2, ptr %1, align 4
  %2 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 1
  store i32 9, ptr %2, align 4
  %3 = call ptr @id(ptr %t)
  %4 = getelementptr inbounds { i32, i32 }, ptr %3, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = call ptr @id(ptr %t)
  %7 = getelementptr inbounds { i32, i32 }, ptr %6, i32 0, i32 1
  %8 = load i32, ptr %7, align 4
  %9 = add i32 %5, %8
  ret i32 %9
}
