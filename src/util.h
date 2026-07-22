/*
 * util.h - small, hardware-independent helpers.
 *
 * These are deliberately free of any hardware/register dependency so they can be
 * unit-tested natively on a host (see tests/).
 */
#ifndef UTIL_H_
#define UTIL_H_

#include <stdint.h>
#include <stddef.h>

// Parse a signed decimal like "1.5", "-0.25", "20". Stops at the first
// non-numeric character. Used instead of scanf("%f") to keep newlib's
// float-scanf support out of the firmware image.
float parse_decimal(const char* s);

// Rotate-xor rolling checksum over n bytes. Detects single-bit flips and byte
// reordering; used to validate stored flash-profile blocks.
uint32_t rolling_checksum(const void* data, size_t n);

#endif /* UTIL_H_ */
