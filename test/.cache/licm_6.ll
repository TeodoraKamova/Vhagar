; ModuleID = 'compile/licm_6.c'
source_filename = "compile/licm_6.c"

define i32 @f(i32 %0, i32 %1, i32 %2) {
  %4 = icmp slt i32 0, %2
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  br label %7

6:                                                ; preds = %3
  br label %7

7:                                                ; preds = %6, %5
  ret i32 0
}

define i32 @main() {
  %1 = call i32 @f(i32 2, i32 3, i32 4)
  %2 = ashr i32 %1, 7
  %3 = lshr i32 %2, 24
  %4 = add i32 %1, %3
  %5 = and i32 %4, -256
  %6 = sub i32 %1, %5
  ret i32 %6
}
