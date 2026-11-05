; ModuleID = 'compile/struct_arrow_access_1.c'
source_filename = "compile/struct_arrow_access_1.c"

define i32 @f(ptr %0) {
  %2 = getelementptr inbounds { i32 }, ptr %0, i32 0, i32 0
  %3 = load i32, ptr %2, align 4
  ret i32 %3
}

define i32 @main() {
  %s = alloca { i32 }, align 8
  %1 = getelementptr inbounds { i32 }, ptr %s, i32 0, i32 0
  store i32 99, ptr %1, align 4
  %2 = tail call i32 @f(ptr %s)
  ret i32 %2
}
