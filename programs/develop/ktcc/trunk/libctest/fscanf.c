#include <stdio.h>
#include <string.h>
#include "test.h"

#define TEST(r, f, x, m) ( \
	errno=0, ((r) = (f)) == (x) || \
	(t_error("%s failed (" m ")\n", #f, r, x, strerror(errno)), 0) )

#define TEST_S(s, x, m) ( \
	!strcmp((s),(x)) || \
	(t_error("[%s] != [%s] (%s)\n", s, x, m), 0) )

static FILE *writetemp(const char *data)
{
	FILE *f = fopen("_tmpfile.tmp", "w+");
	if (!f) return 0;
	if (!fwrite(data, strlen(data), 1, f)) {
		fclose(f);
		return 0;
	}
	rewind(f);
	return f;
}

int main(void)
{
	int i, x, y;
	double u;
	char a[100], b[100];
	FILE *f;


	TEST(i, !!(f=writetemp("hello, world")), 1, "failed to make temp file");
	if (f) {
		TEST(i, fscanf(f, "%s %[own]", a, b), 2, "got %d fields, expected %d");
		TEST_S(a, "hello,", "wrong result for %s");
		TEST_S(b, "wo", "wrong result for %[own]");
		TEST(i, fgetc(f), 'r', "'%c' != '%c') (%s)");
		fclose(f);
	}

	TEST(i, !!(f=writetemp("ld 0x12 0x34")), 1, "failed to make temp file");
	if (f) {
		TEST(i, fscanf(f, "ld %5i%2i", &x, &y), 1, "got %d fields, expected %d");
		TEST(i, x, 0x12, "%d != %d");
		TEST(i, fgetc(f), '3', "'%c' != '%c'");
		fclose(f);
	}

	TEST(i, !!(f=writetemp("      42")), 1, "failed to make temp file");
	if (f) {
		x=y=-1;
		TEST(i, fscanf(f, " %n%*d%n", &x, &y), 0, "%d != %d");
		TEST(i, x, 6, "%d != %d");
		TEST(i, y, 8, "%d != %d");
		TEST(i, ftell(f), 8, "%d != %d");
		TEST(i, !!feof(f), 1, "%d != %d");
		fclose(f);
	}

	TEST(i, !!(f=writetemp("[abc123]....x")), 1, "failed to make temp file");
	if (f) {
		x=y=-1;
		TEST(i, fscanf(f, "%10[^]]%n%10[].]%n", a, &x, b, &y), 2, "%d != %d");
		TEST_S(a, "[abc123", "wrong result for %[^]]");
		TEST_S(b, "]....", "wrong result for %[].]");
		TEST(i, x, 7, "%d != %d");
		TEST(i, y, 12, "%d != %d");
		TEST(i, ftell(f), 12, "%d != %d");
		TEST(i, feof(f), 0, "%d != %d");
		TEST(i, fgetc(f), 'x', "%d != %d");
		fclose(f);
	}

	TEST(i, !!(f=writetemp("0x1.0p12")), 1, "failed to make temp file");
	if (f) {
		x=y=-1;
		u=-1;
		TEST(i, fscanf(f, "%lf%n %d", &u, &x, &y), 1, "%d != %d");
		TEST(u, u, 0.0, "%g != %g");
		TEST(i, x, 1, "%d != %d");
		TEST(i, y, -1, "%d != %d");
		TEST(i, ftell(f), 1, "%d != %d");
		TEST(i, feof(f), 0, "%d != %d");
		TEST(i, fgetc(f), 'x', "%d != %d");
		rewind(f);
		TEST(i, fgetc(f), '0', "%d != %d");
		TEST(i, fgetc(f), 'x', "%d != %d");
		TEST(i, fscanf(f, "%lf%n%c %d", &u, &x, a, &y), 3, "%d != %d");
		TEST(u, u, 1.0, "%g != %g");
		TEST(i, x, 3, "%d != %d");
		TEST(i, a[0], 'p', "%d != %d");
		TEST(i, y, 12, "%d != %d");
		TEST(i, ftell(f), 8, "%d != %d");
		TEST(i, !!feof(f), 1, "%d != %d");
		fclose(f);
	}

	TEST(i, !!(f=writetemp("1.0       012")), 1, "failed to make temp file");
	if (f) {
		x=y=-1;
		u=-1;
		TEST(i, fscanf(f, "%lf%n %i", &u, &x, &y), 2, "%d != %d");
		TEST(u, u, 1.0, "%g != %g");
		TEST(i, x, 3, "%d != %d");
		TEST(i, y, 10, "%d != %d");
		TEST(i, ftell(f), 13, "%d != %d");
		TEST(i, !!feof(f), 1, "%d != %d");
		fclose(f);
	}

	TEST(i, !!(f=writetemp("0xx")), 1, "failed to make temp file");
	if (f) {
		x=y=-1;
		TEST(i, fscanf(f, "%x%n", &x, &y), 0, "%d != %d");
		TEST(i, x, -1, "%d != %d");
		TEST(i, y, -1, "%d != %d");
		TEST(i, ftell(f), 2, "%d != %d");
		TEST(i, feof(f), 0, "%d != %d");
		fclose(f);
	}

	printf("%s finished\n", __FILE__);
	return t_status;
}
