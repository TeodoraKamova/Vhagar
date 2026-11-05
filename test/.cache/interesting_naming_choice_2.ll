; ModuleID = 'compile/interesting_naming_choice_2.c'
source_filename = "compile/interesting_naming_choice_2.c"

define i32 @foo(i32 %0) {
  ret i32 %0
}

define i32 @main() {
  %1 = tail call i32 @foo(i32 0)
  ret i32 %1
}
