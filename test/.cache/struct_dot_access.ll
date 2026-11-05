; ModuleID = 'compile/struct_dot_access.c'
source_filename = "compile/struct_dot_access.c"

define i32 @main() {
  %s = alloca { i32 }, align 8
  %1 = getelementptr inbounds { i32 }, ptr %s, i32 0, i32 0
  store i32 99, ptr %1, align 4
  %2 = getelementptr inbounds { i32 }, ptr %s, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  ret i32 %3
}
