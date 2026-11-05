; ModuleID = 'compile/hello_world.c'
source_filename = "compile/hello_world.c"

@0 = private unnamed_addr constant [14 x i8] c"Hello, World!\00", align 1

declare i32 @puts(ptr)

define i32 @main() {
  %1 = tail call i32 @puts(ptr @0)
  ret i32 %1
}
