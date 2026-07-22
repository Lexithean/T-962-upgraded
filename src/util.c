/*
 * util.c - small, hardware-independent helpers (see util.h).
 */
#include "util.h"

float parse_decimal(const char* s) {
	while (*s == ' ') s++;
	int neg = 0;
	if (*s == '-') { neg = 1; s++; }
	else if (*s == '+') { s++; }
	float val = 0.0f;
	while (*s >= '0' && *s <= '9') { val = val * 10.0f + (float)(*s - '0'); s++; }
	if (*s == '.') {
		s++;
		float frac = 0.1f;
		while (*s >= '0' && *s <= '9') { val += (float)(*s - '0') * frac; frac *= 0.1f; s++; }
	}
	return neg ? -val : val;
}

uint32_t rolling_checksum(const void* data, size_t n) {
	uint32_t c = 0x1234ABCDu;
	const uint8_t* p = (const uint8_t*)data;
	for (size_t i = 0; i < n; i++) {
		c = ((c << 1) | (c >> 31)) ^ p[i];
	}
	return c;
}
