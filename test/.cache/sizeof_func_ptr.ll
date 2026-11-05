; ModuleID = 'compile/sizeof_func_ptr.c'
source_filename = "compile/sizeof_func_ptr.c"

define i32 @main() {
  %fp = alloca ptr, align 8
  br label %1

1:                                                ; preds = %0
  ret i32 8
}
