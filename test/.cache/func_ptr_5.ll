; ModuleID = 'compile/func_ptr_5.c'
source_filename = "compile/func_ptr_5.c"

@f = common global ptr null, align 8

define i32 @foo(i32 %0, i32 %1) {
  %3 = add i32 %0, %1
  ret i32 %3
}

define i32 @main() {
  store ptr @foo, ptr @f, align 8
  %1 = load ptr, ptr @f, align 8
  %2 = tail call i32 %1(i32 2, i32 9)
  ret i32 %2
}
