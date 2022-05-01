define i32 @Gcd_GCD(i32 %0, i32 %1) {
2:
	%3 = alloca i32, align 4
	%4 = alloca i32, align 4
	%5 = alloca i32, align 4
	store i32 %0, i32* %3, align 4
	store i32 %1, i32* %4, align 4
	br label %while_cond_5
while_cond_5:
	%6 = load i32, i32* %4, align 4
	%7 = icmp ne i32 %6, 0
	br i1 %7, label %while_body_5, label %while_after_5
while_body_5:
	%8 = load i32, i32* %3, align 4
	%9 = load i32, i32* %4, align 4
	%10 = srem i32 %8, %9
	store i32 %10, i32* %5, align 4
	%11 = load i32, i32* %4, align 4
	store i32 %11, i32* %3, align 4
	%12 = load i32, i32* %5, align 4
	store i32 %12, i32* %4, align 4
	br label %while_cond_5
while_after_5:
	%13 = load i32, i32* %3, align 4
	ret i32 %13
}
define void @Gcd__init() {
0:
	ret void
}
