; ModuleID = 'compile/void.c'
source_filename = "compile/void.c"

define void @f() {
  ret void
}

define i32 @main() {
  call void @f()
  ret i32 99
}
