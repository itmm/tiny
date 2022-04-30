extern int Gcd_GCD(int, int);

#include <stdio.h>
#include <assert.h>

int run(int a, int b, int ex) {
	int got = Gcd_GCD(a, b);
	printf("gcd(%d, %d) == %d\n", a, b, got);
	assert(got == ex);
}

int main() {
	run(100, 30, 10);
}
