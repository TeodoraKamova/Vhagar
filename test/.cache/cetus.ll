; ModuleID = 'compile/cetus.c'
source_filename = "compile/cetus.c"

define i32 @foo() {
  ret i32 1
}

define i32 @main() {
  %1 = call i32 @foo()
  %2 = sub i32 0, %1
  %3 = icmp ne i32 %2, 0
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  ret i32 0

5:                                                ; preds = %0
  ret i32 1
}
