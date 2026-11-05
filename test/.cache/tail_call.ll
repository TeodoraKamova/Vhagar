; ModuleID = 'compile/tail_call.c'
source_filename = "compile/tail_call.c"

define i32 @fac_helper(i32 %0, i32 %1) {
  %3 = icmp sle i32 %0, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  ret i32 %1

5:                                                ; preds = %2
  %6 = sub i32 %0, 1
  %7 = mul i32 %0, %1
  %8 = call i32 @fac_helper(i32 %6, i32 %7)
  ret i32 %8
}

define i32 @fac(i32 %0) {
  %2 = call i32 @fac_helper(i32 %0, i32 1)
  ret i32 %2
}

define i32 @main() {
  %1 = call i32 @fac(i32 6)
  ret i32 %1
}
