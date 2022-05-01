extern void Gcd__init();
extern int Gcd_GCD(int, int);
extern int Gcd_Min(int, int);

#include <stdio.h>
#include <assert.h>

void run_gcd(int a, int b, int ex) {
	int got = Gcd_GCD(a, b);
	printf("gcd(%d, %d) == %d\n", a, b, got);
	assert(got == ex);
}

void run_min(int a, int b, int ex) {
	int got = Gcd_Min(a, b);
	printf("min(%d, %d) == %d\n", a, b, got);
	assert(got == ex);
}

int main() {
	Gcd__init();
	run_gcd(100, 30, 10);
	run_gcd(32, 4, 4);
	run_gcd(17, 3, 1);
	run_min(10, 30, 10);
	run_min(30, 10, 10);
	run_min(11, 11, 11);
}
