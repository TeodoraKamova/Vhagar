; ModuleID = 'compile/sccp_dead_code.c'
source_filename = "compile/sccp_dead_code.c"

define i32 @foo(i32 %0, i32 %1, i32 %2) {
  %4 = alloca i32, align 4
  store i32 %0, ptr %4, align 4
  %5 = alloca i32, align 4
  store i32 %1, ptr %5, align 4
  %6 = alloca i32, align 4
  store i32 %2, ptr %6, align 4
  br label %7

7:                                                ; preds = %3
  %8 = load i32, ptr %4, align 4
  %9 = load i32, ptr %5, align 4
  %10 = add i32 %8, %9
  %11 = load i32, ptr %6, align 4
  %12 = add i32 %10, %11
  ret i32 %12
}

define i32 @main() {
  %a = alloca i32, align 4
  %b = alloca i32, align 4
  %c = alloca i32, align 4
  %d = alloca i32, align 4
  %undef = alloca i32, align 4
  %dead = alloca i32, align 4
  br label %1

1:                                                ; preds = %0
  store i32 1, ptr %a, align 4
  store i32 1, ptr %b, align 4
  %2 = load i32, ptr %a, align 4
  %3 = load i32, ptr %b, align 4
  %4 = add i32 %2, %3
  store i32 %4, ptr %c, align 4
  %5 = load i32, ptr %c, align 4
  %6 = add i32 %5, 1
  store i32 %6, ptr %d, align 4
  %7 = load i32, ptr %d, align 4
  store i32 %7, ptr %b, align 4
  %8 = load i32, ptr %a, align 4
  %9 = load i32, ptr %b, align 4
  %10 = load i32, ptr %c, align 4
  %11 = mul i32 %9, %10
  %12 = add i32 %8, %11
  %13 = load i32, ptr %d, align 4
  %14 = sub i32 %12, %13
  store i32 %14, ptr %dead, align 4
  br label %cond

15:                                               ; preds = %cond
  %16 = load i32, ptr %dead, align 4
  %17 = add i32 %16, 1
  store i32 %17, ptr %dead, align 4
  br label %cond

18:                                               ; preds = %cond
  %19 = load i32, ptr %dead, align 4
  %20 = add i32 %19, 1
  store i32 %20, ptr %dead, align 4
  %21 = load i32, ptr %dead, align 4
  %22 = add i32 %21, 1
  store i32 %22, ptr %dead, align 4
  %23 = load i32, ptr %dead, align 4
  %24 = mul i32 48, %23
  %25 = mul i32 %24, 2
  %26 = add i32 %25, 4
  store i32 %26, ptr %dead, align 4
  %27 = load i32, ptr %a, align 4
  %28 = load i32, ptr %b, align 4
  %29 = load i32, ptr %c, align 4
  %30 = call i32 @foo(i32 %27, i32 %28, i32 %29)
  ret i32 %30

cond:                                             ; preds = %15, %1
  %31 = load i32, ptr %undef, align 4
  %32 = icmp ne i32 %31, 0
  br i1 %32, label %15, label %18
}
