; ModuleID = 'compile/struct_cast.c'
source_filename = "compile/struct_cast.c"

define i32 @main() {
  %s = alloca { i32 }, align 8
  %1 = getelementptr inbounds { i32 }, ptr %s, i32 0, i32 0
  store i32 9, ptr %1, align 4
  %2 = getelementptr inbounds { i32 }, ptr %s, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  %4 = getelementptr inbounds { i32 }, ptr %s, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = add i32 %3, %5
  %7 = sub i32 %6, 18
  ret i32 %7
}
