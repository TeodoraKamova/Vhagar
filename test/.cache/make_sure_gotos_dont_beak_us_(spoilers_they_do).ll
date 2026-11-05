; ModuleID = 'compile/make_sure_gotos_dont_beak_us_(spoilers_they_do).c'
source_filename = "compile/make_sure_gotos_dont_beak_us_(spoilers_they_do).c"

define i32 @main() {
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  br label %1

1:                                                ; preds = %0
  store i32 1024, ptr %a, align 4
  %2 = load i32, ptr %a, align 4
  %3 = shl i32 %2, 2
  store i32 %3, ptr %b, align 4
  br label %4

4:                                                ; preds = %13, %1
  store i32 0, ptr %c, align 4
  br label %cond

5:                                                ; preds = %cond
  %6 = load i32, ptr %a, align 4
  %7 = add i32 %6, 2
  store i32 %7, ptr %a, align 4
  br label %13

8:                                                ; preds = %cond
  %9 = load i32, ptr %c, align 4
  ret i32 %9

cond:                                             ; preds = %4
  %10 = load i32, ptr %a, align 4
  %11 = load i32, ptr %b, align 4
  %12 = icmp ne i32 %10, %11
  br i1 %12, label %5, label %8

13:                                               ; preds = %5
  %14 = load i32, ptr %b, align 4
  store i32 %14, ptr %a, align 4
  br label %4
  ret i32 1
}
