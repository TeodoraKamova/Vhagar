; ModuleID = 'compile/func_ptr_in_struct.c'
source_filename = "compile/func_ptr_in_struct.c"

define i32 @foo(i32 %0, i32 %1) {
  %3 = add i32 %0, %1
  ret i32 %3
}

define i32 @main() {
  %s = alloca { ptr }, align 8
  %1 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  store ptr @foo, ptr %1, align 8
  %2 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  %3 = load ptr, ptr %2, align 8
  %4 = call i32 %3(i32 2, i32 9)
  %5 = getelementptr inbounds { ptr }, ptr %s, i32 0, i32 0
  %6 = load ptr, ptr %5, align 8
  %7 = call i32 %6(i32 9, i32 2)
  %8 = add i32 %4, %7
  ret i32 %8
}
