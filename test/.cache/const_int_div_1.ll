; ModuleID = 'compile/const_int_div_1.c'
source_filename = "compile/const_int_div_1.c"

define i32 @foo(i32 %0) {
  %2 = lshr i32 %0, 31
  %3 = add i32 %0, %2
  %4 = ashr i32 %3, 3
  ret i32 %4
}

define i32 @main() {
  ret i32 0
}
