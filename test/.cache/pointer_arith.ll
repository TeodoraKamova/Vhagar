; ModuleID = 'compile/pointer_arith.c'
source_filename = "compile/pointer_arith.c"

define i32 @main() {
  %1 = getelementptr inbounds i32, ptr null, i32 1
  %2 = getelementptr inbounds i8, ptr null, i32 4
  %3 = icmp ne ptr %1, %2
  br i1 %3, label %4, label %5

4:                                                ; preds = %0
  ret i32 3

5:                                                ; preds = %0
  ret i32 0
}
