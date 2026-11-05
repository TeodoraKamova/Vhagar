; ModuleID = 'compile/const_int_rem_1.c'
source_filename = "compile/const_int_rem_1.c"

define i32 @foo(i32 %0) {
  %2 = ashr i32 %0, 2
  %3 = lshr i32 %2, 29
  %4 = add i32 %0, %3
  %5 = and i32 %4, -8
  %6 = sub i32 %0, %5
  ret i32 %6
}

define i32 @main() {
  ret i32 0
}
