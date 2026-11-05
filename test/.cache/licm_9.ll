; ModuleID = 'compile/licm_9.c'
source_filename = "compile/licm_9.c"

define void @side_effect(ptr %0) {
  store i32 4, ptr %0, align 4
  ret void
}

define i32 @f(i32 %0, i32 %1, i32 %2) {
entry:
  %i = alloca i32, align 4
  store i32 0, ptr %i, align 4
  %3 = sub i32 %0, %1
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %4 = load i32, ptr %i, align 4
  %5 = mul i32 %4, %3
  %6 = add i32 %s.0, %5
  call void @side_effect(ptr %i)
  %7 = load i32, ptr %i, align 4
  %8 = add i32 %7, 1
  store i32 %8, ptr %i, align 4
  br label %licm-header

9:                                                ; preds = %licm-header
  ret i32 %s.0

licm-header:                                      ; preds = %entry, %licm-latch
  %s.0 = phi i32 [ %6, %licm-latch ], [ 0, %entry ]
  %10 = load i32, ptr %i, align 4
  %11 = icmp slt i32 %10, %2
  br i1 %11, label %licm-latch, label %9
}

define i32 @main() {
  %1 = call i32 @f(i32 2, i32 3, i32 4)
  %2 = ashr i32 %1, 7
  %3 = lshr i32 %2, 24
  %4 = add i32 %1, %3
  %5 = and i32 %4, -256
  %6 = sub i32 %1, %5
  ret i32 %6
}
