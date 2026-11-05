; ModuleID = 'compile/inliner_13.c'
source_filename = "compile/inliner_13.c"

@0 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare void @c4_print_int(i32)

define void @call_me_maybe() {
  %1 = call i32 (ptr, ...) @printf(ptr @0, i32 0)
  ret void
}

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  call void @call_me_maybe()
  br label %5

5:                                                ; preds = %4, %2
  ret i32 0
}

declare i32 @printf(ptr, ...)
