; ModuleID = 'compile/struct_arrow_access_2.c'
source_filename = "compile/struct_arrow_access_2.c"

define i32 @main() {
  %s = alloca { i32, i32 }, align 8
  %1 = getelementptr inbounds { i32, i32 }, ptr %s, i32 0, i32 1
  store i32 99, ptr %1, align 4
  %2 = getelementptr inbounds { i32, i32 }, ptr %s, i32 0, i32 1
  %3 = load i32, ptr %2, align 4
  ret i32 %3
}
