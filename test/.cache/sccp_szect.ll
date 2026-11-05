; ModuleID = 'compile/sccp_szect.c'
source_filename = "compile/sccp_szect.c"

define i32 @foo(i32 %0) {
  ret i32 %0
}

define i32 @main() {
  %1 = call i32 @foo(i32 97)
  %2 = add i32 48, %1
  %3 = sub i32 %2, 145
  ret i32 %3
}
