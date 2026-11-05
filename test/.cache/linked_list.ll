; ModuleID = 'compile/linked_list.c'
source_filename = "compile/linked_list.c"

@0 = private unnamed_addr constant [2 x i8] c"b\00", align 1

declare ptr @c4_malloc(i32)

define ptr @create(ptr %0, ptr %1) {
  %3 = tail call ptr @malloc(i32 16)
  %4 = getelementptr inbounds { ptr, ptr }, ptr %3, i32 0, i32 1
  store ptr %0, ptr %4, align 8
  %5 = getelementptr inbounds { ptr, ptr }, ptr %3, i32 0, i32 0
  store ptr %1, ptr %5, align 8
  ret ptr %3
}

define i32 @main() {
  %1 = call ptr @create(ptr @0, ptr null)
  ret i32 0
}

declare ptr @malloc(i32)
