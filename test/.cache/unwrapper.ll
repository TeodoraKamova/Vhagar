; ModuleID = 'compile/unwrapper.c'
source_filename = "compile/unwrapper.c"

@g = common global i32 0, align 4

define void @foo(i32 %0) {
  store i32 %0, ptr @g, align 4
  ret void
}

define i32 @main() {
  store i32 1, ptr @g, align 4
  call void @foo(i32 0)
  %1 = load i32, ptr @g, align 4
  ret i32 %1
}
