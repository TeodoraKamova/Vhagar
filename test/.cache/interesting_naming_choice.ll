; ModuleID = 'compile/interesting_naming_choice.c'
source_filename = "compile/interesting_naming_choice.c"

define i32 @foo() {
  ret i32 0
}

define i32 @main() {
  %1 = tail call i32 @foo()
  ret i32 %1
}
