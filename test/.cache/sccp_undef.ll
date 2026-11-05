; ModuleID = 'compile/sccp_undef.c'
source_filename = "compile/sccp_undef.c"

define i32 @foo() {
  ret i32 1
}

define i32 @main() {
  %1 = call i32 @foo()
  %2 = sub i32 %1, 1
  ret i32 %2
}
