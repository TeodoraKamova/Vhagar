; ModuleID = 'compile/sccp_trunc.c'
source_filename = "compile/sccp_trunc.c"

define i32 @foo(ptr %0) {
  %2 = getelementptr inbounds i8, ptr %0, i32 0
  %3 = load i8, ptr %2, align 1
  %4 = sext i8 %3 to i32
  ret i32 %4
}

define i32 @main() {
  %b = alloca i8, align 1
  store i8 77, ptr %b, align 1
  %1 = call i32 @foo(ptr %b)
  %2 = sub i32 %1, 77
  ret i32 %2
}
