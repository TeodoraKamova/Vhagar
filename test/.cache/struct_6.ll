; ModuleID = 'compile/struct_6.c'
source_filename = "compile/struct_6.c"

define { i32, i32 } @f1(ptr %0) {
  %2 = load { i32, i32 }, ptr %0, align 4
  ret { i32, i32 } %2
}

define ptr @f2(ptr %0) {
  ret ptr %0
}

define i32 @main() {
  %t = alloca { i32, i32 }, align 8
  %1 = alloca { i32, i32 }, align 8
  %2 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 0
  store i32 2, ptr %2, align 4
  %3 = getelementptr inbounds { i32, i32 }, ptr %t, i32 0, i32 1
  store i32 9, ptr %3, align 4
  %4 = call { i32, i32 } @f1(ptr %t)
  store { i32, i32 } %4, ptr %1, align 4
  %5 = getelementptr inbounds { i32, i32 }, ptr %1, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  %7 = call ptr @f2(ptr %t)
  %8 = getelementptr inbounds { i32, i32 }, ptr %7, i32 0, i32 1
  %9 = load i32, ptr %8, align 4
  %10 = add i32 %6, %9
  ret i32 %10
}
