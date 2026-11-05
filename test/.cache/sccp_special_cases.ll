; ModuleID = 'compile/sccp_special_cases.c'
source_filename = "compile/sccp_special_cases.c"

define i32 @sc1(i32 %0) {
  ret i32 1
}

define i32 @no1(i32 %0, i32 %1) {
  %3 = sdiv i32 %0, %1
  ret i32 %3
}

define i32 @sc2(i32 %0) {
  ret i32 0
}

define i32 @sc3(i32 %0) {
  ret i32 undef
}

define i32 @sc4(i32 %0) {
  ret i32 0
}

define i32 @sc5(i32 %0) {
  ret i32 0
}

define i32 @sc6(i32 %0) {
  ret i32 0
}

define i32 @no2(i32 %0, i32 %1) {
  %3 = srem i32 %0, %1
  ret i32 %3
}

define i32 @sc7(i32 %0) {
  ret i32 0
}

define i32 @sc8(i32 %0) {
  ret i32 0
}

define i32 @sc9(i32 %0) {
  ret i32 0
}

define i32 @scA(i32 %0) {
  ret i32 0
}

define i32 @no3(i32 %0) {
  %2 = add i32 %0, %0
  ret i32 %2
}

define i32 @scB(i32 %0) {
  %2 = add i32 %0, -1
  ret i32 %2
}

define i32 @scC(i32 %0) {
  %2 = add i32 %0, -1
  ret i32 %2
}

define i32 @main() {
  ret i32 0
}
