; ModuleID = 'compile/sizeof_3.c'
source_filename = "compile/sizeof_3.c"

define i32 @main() {
  %s = alloca { ptr, i32, i8 }, align 8
  br label %1

1:                                                ; preds = %0
  ret i32 16
}
