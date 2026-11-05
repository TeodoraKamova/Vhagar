; ModuleID = 'compile/sizeof_4.c'
source_filename = "compile/sizeof_4.c"

define i32 @main() {
  %s = alloca ptr, align 8
  br label %1

1:                                                ; preds = %0
  ret i32 8
}
