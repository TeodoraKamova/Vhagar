; ModuleID = 'compile/sizeof_cmp.c'
source_filename = "compile/sizeof_cmp.c"

@a = common global i32 0, align 4
@b = common global i32 0, align 4

define i32 @main() {
  store i32 2, ptr @a, align 4
  store i32 2, ptr @b, align 4
  %1 = load i32, ptr @b, align 4
  %2 = shl i32 %1, 2
  %3 = sub i32 %2, %1
  store i32 %3, ptr @a, align 4
  store i32 0, ptr @a, align 4
  ret i32 0
}
