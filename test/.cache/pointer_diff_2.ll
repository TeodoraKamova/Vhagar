; ModuleID = 'compile/pointer_diff_2.c'
source_filename = "compile/pointer_diff_2.c"

define i32 @f(ptr %0) {
  %2 = getelementptr inbounds i8, ptr %0, i32 100
  %3 = ptrtoint ptr %2 to i64
  %4 = ptrtoint ptr %0 to i64
  %5 = sub i64 %3, %4
  %6 = sdiv exact i64 %5, ptrtoint (ptr getelementptr (i8, ptr null, i32 1) to i64)
  %7 = trunc i64 %6 to i32
  ret i32 %7
}

define i32 @main() {
  %1 = tail call i32 @f(ptr undef)
  ret i32 %1
}
