; ModuleID = 'compile/sizeof_shadowed.c'
source_filename = "compile/sizeof_shadowed.c"

@0 = private unnamed_addr constant [2 x i8] c" \00", align 1

declare void @c4_print_int(i32)

declare void @c4_print_str(ptr)

define i32 @main() {
  %i = alloca i32, align 4
  %res = alloca i32, align 4
  %res1 = alloca i8, align 1
  br label %1

1:                                                ; preds = %0
  store i32 0, ptr %res, align 4
  store i32 0, ptr %i, align 4
  br label %cond

2:                                                ; preds = %cond
  store i8 4, ptr %res, align 1
  %3 = load i32, ptr %i, align 4
  %4 = add i32 %3, 1
  store i32 %4, ptr %i, align 4
  %5 = load i32, ptr %i, align 4
  call void @c4_print_int(i32 %5)
  call void @c4_print_str(ptr @0)
  br label %cond

6:                                                ; preds = %cond
  %7 = load i32, ptr %res, align 4
  ret i32 %7

cond:                                             ; preds = %2, %1
  %8 = load i32, ptr %i, align 4
  %9 = icmp slt i32 %8, 10
  br i1 %9, label %2, label %6
}
