; ModuleID = 'compile/struct_nested_arrow_access.c'
source_filename = "compile/struct_nested_arrow_access.c"

define i32 @main() {
  %m = alloca { i32, ptr }, align 8
  %n = alloca { i32, i32 }, align 8
  %1 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 1
  store ptr %n, ptr %1, align 8
  %2 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 0
  store i32 9, ptr %2, align 4
  %3 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 1
  %4 = load ptr, ptr %3, align 8
  %5 = getelementptr inbounds { i32, i32 }, ptr %4, i32 0, i32 0
  store i32 4, ptr %5, align 4
  %6 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 1
  %7 = load ptr, ptr %6, align 8
  %8 = getelementptr inbounds { i32, i32 }, ptr %7, i32 0, i32 1
  store i32 6, ptr %8, align 4
  %9 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 0
  %10 = load i32, ptr %9, align 4
  %11 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 1
  %12 = load ptr, ptr %11, align 8
  %13 = getelementptr inbounds { i32, i32 }, ptr %12, i32 0, i32 0
  %14 = load i32, ptr %13, align 4
  %15 = add i32 %10, %14
  %16 = getelementptr inbounds { i32, ptr }, ptr %m, i32 0, i32 1
  %17 = load ptr, ptr %16, align 8
  %18 = getelementptr inbounds { i32, i32 }, ptr %17, i32 0, i32 1
  %19 = load i32, ptr %18, align 4
  %20 = add i32 %15, %19
  ret i32 %20
}
