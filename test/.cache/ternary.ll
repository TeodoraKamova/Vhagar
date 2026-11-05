; ModuleID = 'compile/ternary.c'
source_filename = "compile/ternary.c"

define i32 @main(i32 %0, ptr %1) {
  %3 = lshr i32 %0, 31
  %4 = add i32 %0, %3
  %5 = and i32 %4, -2
  %6 = sub i32 %0, %5
  %7 = icmp eq i32 %6, 0
  %8 = select i1 %7, i32 19, i32 21
  ret i32 %8
}
