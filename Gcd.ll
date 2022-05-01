
define i32 @Gcd_GCD(i32 %0, i32 %1) {
entry:
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %2, align 4
  store i32 %1, i32* %3, align 4
  br label %while.cond

while.cond:                                       ; preds = %while.body, %entry
  %5 = load i32, i32* %3, align 4
  %6 = icmp ne i32 %5, 0
  br i1 %6, label %while.body, label %while.after

while.body:                                       ; preds = %while.cond
  %7 = load i32, i32* %2, align 4
  %8 = load i32, i32* %3, align 4
  %9 = srem i32 %7, %8
  store i32 %9, i32* %4, align 4
  %10 = load i32, i32* %3, align 4
  store i32 %10, i32* %2, align 4
  %11 = load i32, i32* %4, align 4
  store i32 %11, i32* %3, align 4
  br label %while.cond

while.after:                                      ; preds = %while.cond
  %12 = load i32, i32* %2, align 4
  ret i32 %12
}

define void @Gcd__init() {
entry:
  ret void
}
