; ModuleID = 'compile/struct_7.c'
source_filename = "compile/struct_7.c"

define i32 @main() {
  %s = alloca { ptr }, align 8
  %t = alloca { i32 }, align 8
  %1 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  store ptr %t, ptr %1, align 8
  %2 = getelementptr inbounds { i32 }, ptr %t, i32 0, i32 0
  store i32 11, ptr %2, align 4
  %3 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  %4 = load ptr, ptr %3, align 8
  %5 = getelementptr inbounds { i32 }, ptr %4, i32 0, i32 0
  %6 = load i32, ptr %5, align 4
  ret i32 %6
}
