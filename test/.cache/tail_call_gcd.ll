; ModuleID = 'compile/tail_call_gcd.c'
source_filename = "compile/tail_call_gcd.c"

define i32 @gcd(i32 %0, i32 %1) {
  %3 = icmp eq i32 %1, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  ret i32 %0

5:                                                ; preds = %2
  %6 = srem i32 %0, %1
  %7 = tail call i32 @gcd(i32 %1, i32 %6)
  ret i32 %7
}

define i32 @main() {
  %1 = tail call i32 @gcd(i32 1596, i32 5648)
  ret i32 %1
}
