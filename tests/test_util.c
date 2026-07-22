/*
 * test_util.c - native unit tests for the hardware-independent helpers in
 * src/util.c. Built and run on the host (CI runs these on every push/PR).
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "../src/util.h"

static int failures = 0;
static int checks = 0;

#define CHECK(cond) do { \
	checks++; \
	if (!(cond)) { failures++; printf("FAIL %s:%d  %s\n", __FILE__, __LINE__, #cond); } \
} while (0)

#define CHECK_FEQ(a, b) do { \
	checks++; \
	if (fabs((double)(a) - (double)(b)) > 1e-4) { \
		failures++; printf("FAIL %s:%d  %s (%f) != %s (%f)\n", __FILE__, __LINE__, #a, (double)(a), #b, (double)(b)); \
	} \
} while (0)

static void test_parse_decimal(void) {
	CHECK_FEQ(parse_decimal("20"), 20.0f);
	CHECK_FEQ(parse_decimal("0"), 0.0f);
	CHECK_FEQ(parse_decimal("1.5"), 1.5f);
	CHECK_FEQ(parse_decimal("-0.25"), -0.25f);
	CHECK_FEQ(parse_decimal("+3.75"), 3.75f);
	CHECK_FEQ(parse_decimal("  42.5"), 42.5f);   // leading spaces
	CHECK_FEQ(parse_decimal("127.0"), 127.0f);   // PID Kp max
	CHECK_FEQ(parse_decimal("0.508"), 0.508f);   // PID Ki max
	CHECK_FEQ(parse_decimal("100abc"), 100.0f);  // stops at non-numeric
	CHECK_FEQ(parse_decimal("-63.5"), -63.5f);   // TC offset min
}

static void test_rolling_checksum(void) {
	const char a[] = "hello world profile data";
	const char b[] = "hello world profile data";
	const char c[] = "hello world profile datA"; // one bit different

	// Deterministic: same input -> same checksum.
	CHECK(rolling_checksum(a, sizeof(a)) == rolling_checksum(b, sizeof(b)));

	// Sensitive: a single changed byte changes the checksum (corruption detected).
	CHECK(rolling_checksum(a, sizeof(a)) != rolling_checksum(c, sizeof(c)));

	// Order-sensitive: swapping two bytes changes the checksum.
	char d[] = "AB";
	char e[] = "BA";
	CHECK(rolling_checksum(d, 2) != rolling_checksum(e, 2));

	// Single-bit flip anywhere in a 256-byte block is detected.
	unsigned char blk[256];
	for (int i = 0; i < 256; i++) blk[i] = (unsigned char)(i * 7 + 3);
	uint32_t base = rolling_checksum(blk, sizeof(blk));
	int missed = 0;
	for (int i = 0; i < 256; i++) {
		unsigned char save = blk[i];
		blk[i] ^= 0x01;
		if (rolling_checksum(blk, sizeof(blk)) == base) missed++;
		blk[i] = save;
	}
	CHECK(missed == 0);
}

int main(void) {
	test_parse_decimal();
	test_rolling_checksum();
	printf("%d checks, %d failures\n", checks, failures);
	return failures ? 1 : 0;
}
