; ModuleID = 'compile/if_stmt_4.c'
source_filename = "compile/if_stmt_4.c"

define i32 @f() {
  ret i32 99
}

define i32 @main() {
  %1 = call i32 @f()
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 99

4:                                                ; preds = %0
  ret i32 1
}
