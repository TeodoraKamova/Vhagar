; ModuleID = 'compile/if_stmt_1.c'
source_filename = "compile/if_stmt_1.c"

define i32 @main(i32 %0, ptr %1) {
  %3 = icmp eq i32 %0, 1
  br i1 %3, label %4, label %5

4:                                                ; preds = %2
  br label %5

5:                                                ; preds = %4, %2
  %res.0 = phi i32 [ 15, %4 ], [ 7, %2 ]
  ret i32 %res.0
}
