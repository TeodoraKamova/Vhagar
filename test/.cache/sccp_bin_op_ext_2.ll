; ModuleID = 'compile/sccp_bin_op_ext_2.c'
source_filename = "compile/sccp_bin_op_ext_2.c"

define i32 @scB(i32 %0) {
  %2 = add i32 %0, -1
  ret i32 %2
}

define i32 @main() {
  %1 = tail call i32 @scB(i32 1)
  ret i32 %1
}
