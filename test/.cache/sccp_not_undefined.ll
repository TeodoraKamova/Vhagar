; ModuleID = 'compile/sccp_not_undefined.c'
source_filename = "compile/sccp_not_undefined.c"

define void @foo(ptr %0) {
  store i32 0, ptr %0, align 4
  ret void
}

define void @boo(ptr %0) {
  store i32 1, ptr %0, align 4
  ret void
}

define i32 @main() {
  %res = alloca i32, align 4
  call void @boo(ptr %res)
  call void @foo(ptr %res)
  %1 = load i32, ptr %res, align 4
  ret i32 %1
}
