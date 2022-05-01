extern void Gcd__init();
extern int Gcd_GCD(int, int);
extern int Gcd_Min(int, int);
extern int Gcd_Sum(int, int);
extern double Gcd_Summ(double, double);

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

void run_sum(int a, int b, int ex) {
	int got = Gcd_Sum(a, b);
	printf("sum(%d, %d) == %d\n", a, b, got);
	assert(got == ex);
}

void run_summ(double a, double b, double ex) {
	double got = Gcd_Summ(a, b);
	printf("sum(%f, %f) == %f\n", a, b, got);
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
	run_sum(3, 4, 7);
	run_summ(3.0, 4.0, 7.0);
}
