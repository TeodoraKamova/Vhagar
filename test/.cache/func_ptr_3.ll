; ModuleID = 'compile/func_ptr_3.c'
source_filename = "compile/func_ptr_3.c"

define i32 @apply(ptr %0) {
  %2 = tail call i32 %0(i32 2, i32 9)
  ret i32 %2
}

define i32 @foo(i32 %0, i32 %1) {
  %3 = add i32 %0, %1
  ret i32 %3
}

define i32 @main() {
  %1 = tail call i32 @apply(ptr @foo)
  ret i32 %1
}
