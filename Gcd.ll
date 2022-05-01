define i32 @Gcd_GCD(i32 %0, i32 %1) {
entry:
	%2 = alloca i32, align 4
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	store i32 %0, i32* %2, align 4
	store i32 %1, i32* %3, align 4
	br label %while_cond_0_0
while_cond_0_0:
	%5 = load i32, i32* %3, align 4
	%6 = icmp ne i32 %5, 0
	br i1 %6, label %while_body_0_0, label %while_cond_0_1
while_body_0_0:
	%7 = load i32, i32* %2, align 4
	%8 = load i32, i32* %3, align 4
	%9 = srem i32 %7, %8
	store i32 %9, i32* %4, align 4
	%10 = load i32, i32* %3, align 4
	store i32 %10, i32* %2, align 4
	%11 = load i32, i32* %4, align 4
	store i32 %11, i32* %3, align 4
	br label %while_cond_0_0
while_cond_0_1:
	%12 = load i32, i32* %2, align 4
	ret i32 %12
}
define i32 @Gcd_Min(i32 %0, i32 %1) {
entry:
	%2 = alloca i32, align 4
	br label %if_cond_0_0
if_cond_0_0:
	%3 = icmp slt i32 %0, %1
	br i1 %3, label %if_body_0_0, label %if_cond_0_1
if_body_0_0:
	store i32 %0, i32* %2, align 4
	br label %if_end_0
if_cond_0_1:
	store i32 %1, i32* %2, align 4
	br label %if_end_0
if_end_0:
	%4 = load i32, i32* %2, align 4
	ret i32 %4
}
define i32 @Gcd_Sum(i32 %0, i32 %1) {
entry:
	%2 = add i32 %0, %1
	ret i32 %2
}
define double @Gcd_Summ(double %0, double %1) {
entry:
	%2 = add double %0, %1
	ret double %2
}
define i32 @Gcd_Mod(i32 %0, i32 %1) {
entry:
	%2 = srem i32 %0, %1
	ret i32 %2
}
define void @Gcd__init() {
entry:
	ret void
}
