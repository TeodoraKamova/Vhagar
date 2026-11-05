; ModuleID = 'compile/const_int_mul_1.c'
source_filename = "compile/const_int_mul_1.c"

define i32 @foo(i32 %0) {
  %2 = shl i32 %0, 3
  ret i32 %2
}

define i32 @main() {
  ret i32 0
}
