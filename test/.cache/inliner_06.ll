; ModuleID = 'compile/inliner_06.c'
source_filename = "compile/inliner_06.c"

define i32 @f(i32 %0) {
  ret i32 %0
}

define i32 @main() {
  %1 = tail call i32 @f(i32 0)
  ret i32 %1
}
