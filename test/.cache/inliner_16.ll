; ModuleID = 'compile/inliner_16.c'
source_filename = "compile/inliner_16.c"

define i32 @f(i32 %0) {
  ret i32 %0
}

define i32 @main(i32 %0, ptr %1) {
  %3 = sub i32 %0, 1
  %4 = call i32 @f(i32 %3)
  ret i32 %4
}
