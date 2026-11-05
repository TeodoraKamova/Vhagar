; ModuleID = 'compile/types.c'
source_filename = "compile/types.c"

define i32 @add(i32 %0, i32 %1) {
  %3 = add i32 %0, %1
  ret i32 %3
}

define i32 @multiply(i32 %0, i32 %1) {
  %3 = mul i32 %0, %1
  ret i32 %3
}

define i32 @foo(i32 %0) {
  ret i32 %0
}

declare ptr @convoluted(i32, ptr)

define ptr @plottwist(i32 %0, i32 %1) {
  ret ptr @foo
}

define ptr @get_function_pointer(i8 %0) {
  ret ptr null
}

define void @nothing() {
  ret void
}

define void @what_is_even_happening() {
  tail call void @nothing()
  ret void
}

define ptr @scale(ptr %0, i32 %1) {
  %3 = getelementptr inbounds { i32, i32, i32 }, ptr %0, i32 0, i32 0
  %4 = getelementptr inbounds { i32, i32, i32 }, ptr %0, i32 0, i32 0
  %5 = load i32, ptr %4, align 4
  %6 = mul i32 %5, %1
  store i32 %6, ptr %3, align 4
  %7 = getelementptr inbounds { i32, i32, i32 }, ptr %0, i32 0, i32 1
  %8 = getelementptr inbounds { i32, i32, i32 }, ptr %0, i32 0, i32 1
  %9 = load i32, ptr %8, align 4
  %10 = mul i32 %9, %1
  store i32 %10, ptr %7, align 4
  %11 = getelementptr inbounds { i32, i32, i32 }, ptr %0, i32 0, i32 2
  %12 = getelementptr inbounds { i32, i32, i32 }, ptr %0, i32 0, i32 2
  %13 = load i32, ptr %12, align 4
  %14 = mul i32 %13, %1
  store i32 %14, ptr %11, align 4
  ret ptr %0
}

define i32 @main() {
  %1 = call i32 @add(i32 1, i32 -1)
  %2 = icmp ne i32 %1, 0
  br i1 %2, label %3, label %5

3:                                                ; preds = %0
  %4 = tail call i32 @add(i32 1, i32 2)
  ret i32 %4

5:                                                ; preds = %0
  ret i32 0
}
