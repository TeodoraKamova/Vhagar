; ModuleID = 'compile/spigot_pi.c'
source_filename = "compile/spigot_pi.c"

@0 = private unnamed_addr constant [27 x i8] c"Memory allocation failed.\0A\00", align 1
@1 = private unnamed_addr constant [2 x i8] c".\00", align 1
@2 = private unnamed_addr constant [2 x i8] c"\0A\00", align 1
@3 = private unnamed_addr constant [3 x i8] c"%d\00", align 1

declare ptr @c4_malloc(i32)

declare void @c4_free(ptr)

declare void @c4_print_str(ptr)

declare void @c4_print_int(i32)

define void @spigot(i32 %0) {
entry:
  %1 = shl i32 %0, 1
  %2 = shl i32 %0, 3
  %3 = add i32 %2, %1
  %4 = sext i32 %3 to i64
  %5 = mul i64 %4, 1431655766
  %6 = lshr i64 %5, 32
  %7 = trunc i64 %6 to i32
  %8 = lshr i32 %7, 31
  %9 = add i32 %7, %8
  %10 = add i32 %9, 2
  %11 = shl i32 %10, 2
  %12 = call ptr @malloc(i32 %11)
  %13 = icmp eq ptr %12, null
  br i1 %13, label %14, label %16

14:                                               ; preds = %entry
  %15 = call i32 (ptr, ...) @printf(ptr @0)
  ret void

16:                                               ; preds = %entry
  br label %licm-header

licm-latch:                                       ; preds = %licm-header
  %17 = getelementptr inbounds i32, ptr %12, i32 %i.0
  store i32 2, ptr %17, align 4
  %18 = add i32 %i.0, 1
  br label %licm-header

19:                                               ; preds = %licm-header
  %20 = getelementptr inbounds i32, ptr %12, i32 0
  %21 = getelementptr inbounds i32, ptr %12, i32 0
  %22 = getelementptr inbounds i32, ptr %12, i32 0
  %23 = getelementptr inbounds i32, ptr %12, i32 0
  %24 = getelementptr inbounds i32, ptr %12, i32 0
  %25 = sub i32 %10, 1
  br label %licm-header7

licm-header:                                      ; preds = %16, %licm-latch
  %i.0 = phi i32 [ %18, %licm-latch ], [ 0, %16 ]
  %26 = icmp slt i32 %i.0, %10
  br i1 %26, label %licm-latch, label %19

27:                                               ; preds = %licm-header7
  br label %licm-header4

28:                                               ; preds = %licm-header7
  tail call void @free(ptr %12)
  ret void

licm-header7:                                     ; preds = %19, %licm-latch8
  %i.1 = phi i32 [ %80, %licm-latch8 ], [ 0, %19 ]
  %ft.0 = phi i32 [ %ft.1, %licm-latch8 ], [ 0, %19 ]
  %29 = icmp slt i32 %i.1, %0
  br i1 %29, label %27, label %28

licm-latch5:                                      ; preds = %licm-header4
  %30 = getelementptr inbounds i32, ptr %12, i32 %j.0
  %31 = getelementptr inbounds i32, ptr %12, i32 %j.0
  %32 = load i32, ptr %31, align 4
  %33 = shl i32 %32, 1
  %34 = shl i32 %32, 3
  %35 = add i32 %34, %33
  %36 = add i32 %35, %carry.0
  store i32 %36, ptr %30, align 4
  %37 = getelementptr inbounds i32, ptr %12, i32 %j.0
  %38 = load i32, ptr %37, align 4
  %39 = shl i32 %j.0, 1
  %40 = add i32 %39, 1
  %41 = sdiv i32 %38, %40
  %42 = getelementptr inbounds i32, ptr %12, i32 %j.0
  %43 = getelementptr inbounds i32, ptr %12, i32 %j.0
  %44 = load i32, ptr %43, align 4
  %45 = shl i32 %j.0, 1
  %46 = add i32 %45, 1
  %47 = srem i32 %44, %46
  store i32 %47, ptr %42, align 4
  %48 = mul i32 %41, %j.0
  %49 = sub i32 %j.0, 1
  br label %licm-header4

50:                                               ; preds = %licm-header4
  %51 = load i32, ptr %21, align 4
  %52 = shl i32 %51, 1
  %53 = shl i32 %51, 3
  %54 = add i32 %53, %52
  %55 = add i32 %54, %carry.0
  store i32 %55, ptr %20, align 4
  %56 = load i32, ptr %22, align 4
  %57 = sext i32 %56 to i64
  %58 = mul i64 %57, 1717986919
  %59 = lshr i64 %58, 32
  %60 = trunc i64 %59 to i32
  %61 = ashr i32 %60, 2
  %62 = lshr i32 %61, 31
  %63 = add i32 %61, %62
  %64 = load i32, ptr %24, align 4
  %65 = sext i32 %64 to i64
  %66 = mul i64 %65, 1717986919
  %67 = lshr i64 %66, 32
  %68 = trunc i64 %67 to i32
  %69 = ashr i32 %68, 2
  %70 = lshr i32 %69, 31
  %71 = add i32 %69, %70
  %72 = mul i32 %71, 10
  %73 = sub i32 %64, %72
  store i32 %73, ptr %23, align 4
  %74 = call i32 (ptr, ...) @printf(ptr @3, i32 %63)
  %75 = icmp eq i32 %ft.0, 0
  br i1 %75, label %77, label %licm-latch8

licm-header4:                                     ; preds = %27, %licm-latch5
  %carry.0 = phi i32 [ %48, %licm-latch5 ], [ 0, %27 ]
  %j.0 = phi i32 [ %49, %licm-latch5 ], [ %25, %27 ]
  %76 = icmp sgt i32 %j.0, 0
  br i1 %76, label %licm-latch5, label %50

77:                                               ; preds = %50
  %78 = call i32 (ptr, ...) @printf(ptr @1)
  %79 = add i32 %ft.0, 1
  br label %licm-latch8

licm-latch8:                                      ; preds = %77, %50
  %ft.1 = phi i32 [ %79, %77 ], [ %ft.0, %50 ]
  %80 = add i32 %i.1, 1
  br label %licm-header7
}

define i32 @main() {
  call void @spigot(i32 200)
  %1 = call i32 (ptr, ...) @printf(ptr @2)
  ret i32 0
}

declare ptr @malloc(i32)

declare i32 @printf(ptr, ...)

declare void @free(ptr)
