extern void Gcd__init();
extern int Gcd_GCD(int, int);
extern int Gcd_Min(int, int);

#include <stdio.h>
#include <assert.h>

void run(int a, int b, int ex) {
	int got = Gcd_GCD(a, b);
	printf("gcd(%d, %d) == %d\n", a, b, got);
	assert(got == ex);
}

int main() {
	Gcd__init();
	run(100, 30, 10);
}
