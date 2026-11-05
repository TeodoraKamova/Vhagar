; ModuleID = 'compile/pointer_diff.c'
source_filename = "compile/pointer_diff.c"

define i32 @diff(ptr %0, ptr %1) {
  %3 = ptrtoint ptr %0 to i64
  %4 = ptrtoint ptr %1 to i64
  %5 = sub i64 %3, %4
  %6 = sdiv exact i64 %5, ptrtoint (ptr getelementptr (i32, ptr null, i32 1) to i64)
  %7 = trunc i64 %6 to i32
  ret i32 %7
}

define i32 @main() {
  %x = alloca i32, align 4
  %y = alloca i32, align 4
  %1 = call i32 @diff(ptr %x, ptr %y)
  %2 = shl i32 %1, 1
  %3 = shl i32 %1, 3
  %4 = add i32 %3, %2
  ret i32 %4
}
