; ModuleID = 'compile/sizeof_more.c'
source_filename = "compile/sizeof_more.c"

define i32 @main() {
  %1 = getelementptr inbounds { i32, i32, i32 }, ptr null, i32 7
  %2 = icmp ne ptr %1, inttoptr (i64 84 to ptr)
  br i1 %2, label %3, label %4

3:                                                ; preds = %0
  ret i32 21

4:                                                ; preds = %0
  %5 = ptrtoint ptr %1 to i64
  %6 = ptrtoint ptr null to i64
  %7 = sub i64 %5, %6
  %8 = sdiv exact i64 %7, ptrtoint (ptr getelementptr ({ i32, i32, i32 }, ptr null, i32 1) to i64)
  %9 = trunc i64 %8 to i32
  %10 = icmp ne i32 %9, 7
  br i1 %10, label %11, label %12

11:                                               ; preds = %4
  ret i32 22

12:                                               ; preds = %4
  %13 = ptrtoint ptr null to i64
  %14 = ptrtoint ptr %1 to i64
  %15 = sub i64 %13, %14
  %16 = sdiv exact i64 %15, ptrtoint (ptr getelementptr ({ i32, i32, i32 }, ptr null, i32 1) to i64)
  %17 = trunc i64 %16 to i32
  %18 = icmp ne i32 %17, -7
  br i1 %18, label %19, label %20

19:                                               ; preds = %12
  ret i32 23

20:                                               ; preds = %12
  ret i32 0
}
