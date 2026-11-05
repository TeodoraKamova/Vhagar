; ModuleID = 'compile/inliner_12.c'
source_filename = "compile/inliner_12.c"

define void @do_nothing(i32 %0) {
  %2 = icmp eq i32 %0, 5
  br i1 %2, label %3, label %4

3:                                                ; preds = %1
  ret void

4:                                                ; preds = %1
  ret void
}

define i32 @main() {
  call void @do_nothing(i32 5)
  ret i32 0
}
