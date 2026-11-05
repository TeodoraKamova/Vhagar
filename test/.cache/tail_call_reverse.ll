; ModuleID = 'compile/tail_call_reverse.c'
source_filename = "compile/tail_call_reverse.c"

@0 = private unnamed_addr constant [13 x i8] c"!dlroW olleH\00", align 1
@1 = private unnamed_addr constant [3 x i8] c"%s\00", align 1

declare void @c4_print_str(ptr)

declare i32 @c4_strlen(ptr)

declare ptr @c4_memcpy(ptr, ptr, i32)

declare ptr @c4_malloc(i32)

define void @tail_recursive_reverse(ptr %0, i32 %1, i32 %2) {
  %4 = icmp sge i32 %1, %2
  br i1 %4, label %5, label %6

5:                                                ; preds = %3
  ret void

6:                                                ; preds = %3
  %7 = getelementptr inbounds i8, ptr %0, i32 %1
  %8 = load i8, ptr %7, align 1
  %9 = getelementptr inbounds i8, ptr %0, i32 %1
  %10 = getelementptr inbounds i8, ptr %0, i32 %2
  %11 = load i8, ptr %10, align 1
  store i8 %11, ptr %9, align 1
  %12 = getelementptr inbounds i8, ptr %0, i32 %2
  store i8 %8, ptr %12, align 1
  %13 = add i32 %1, 1
  %14 = sub i32 %2, 1
  tail call void @tail_recursive_reverse(ptr %0, i32 %13, i32 %14)
  ret void
}

define void @reverse(ptr %0) {
  %2 = call i32 @c4_strlen(ptr %0)
  %3 = sub i32 %2, 1
  tail call void @tail_recursive_reverse(ptr %0, i32 0, i32 %3)
  ret void
}

define i32 @main() {
  %1 = call ptr @malloc(i32 52)
  call void @llvm.memcpy.p0.p0.i32(ptr align 1 %1, ptr align 1 @0, i32 13, i1 false)
  call void @reverse(ptr %1)
  %2 = call i32 (ptr, ...) @printf(ptr @1, ptr %1)
  ret i32 0
}

declare ptr @malloc(i32)

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i32(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i32, i1 immarg) #0

declare i32 @printf(ptr, ...)

attributes #0 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
