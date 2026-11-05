; ModuleID = 'compile/inliner_02.c'
source_filename = "compile/inliner_02.c"

define i32 @f(i32 %0) {
  %2 = icmp sgt i32 %0, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %1
  %4 = sub i32 %0, 1
  ret i32 %4

5:                                                ; preds = %1
  ret i32 0
}

define i32 @main() {
  %1 = tail call i32 @f(i32 0)
  ret i32 %1
}
