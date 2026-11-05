; ModuleID = 'compile/inliner_11.c'
source_filename = "compile/inliner_11.c"

@0 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1

declare void @c4_print_str(ptr)

declare ptr @c4_malloc(i32)

declare void @c4_free(ptr)

declare i32 @c4_strlen(ptr)

define void @budget_memcpy(ptr %0, ptr %1, i32 %2) {
entry:
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %3 = load i8, ptr %.01, align 1
  store i8 %3, ptr %.0, align 1
  %4 = getelementptr inbounds i8, ptr %.0, i32 1
  %5 = getelementptr inbounds i8, ptr %.01, i32 1
  %6 = sub i32 %.02, 1
  br label %licm-header

7:                                                ; preds = %licm-header
  ret void

licm-header:                                      ; preds = %entry, %licm-latch
  %.02 = phi i32 [ %6, %licm-latch ], [ %2, %entry ]
  %.01 = phi ptr [ %5, %licm-latch ], [ %1, %entry ]
  %.0 = phi ptr [ %4, %licm-latch ], [ %0, %entry ]
  %8 = icmp sgt i32 %.02, 0
  br i1 %8, label %licm-latch, label %7
}

define i32 @main(i32 %0, ptr %1) {
  %3 = getelementptr inbounds ptr, ptr %1, i32 0
  %4 = load ptr, ptr %3, align 8
  %5 = call i32 @c4_strlen(ptr %4)
  %6 = add i32 %5, 1
  %7 = call ptr @malloc(i32 %6)
  %8 = getelementptr inbounds ptr, ptr %1, i32 0
  %9 = load ptr, ptr %8, align 8
  call void @budget_memcpy(ptr %7, ptr %9, i32 %6)
  %10 = call i32 (ptr, ...) @printf(ptr @0, ptr %7)
  call void @free(ptr %7)
  ret i32 0
}

declare ptr @malloc(i32)

declare i32 @printf(ptr, ...)

declare void @free(ptr)
