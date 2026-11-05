; ModuleID = 'compile/func_ptr_nested.c'
source_filename = "compile/func_ptr_nested.c"

define i32 @inner(i32 %0, i32 %1) {
  ret i32 11
}

define ptr @outer() {
  ret ptr @inner
}

define i32 @main() {
  %1 = call ptr @outer()
  %2 = tail call i32 %1(i32 2, i32 9)
  ret i32 %2
}
