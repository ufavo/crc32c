// Copyright (c) 2026 Luiz Gustavo S Borsoi
// Licensed under the MIT License. See LICENSE file in the project root.

#include "../include/crc32c.h"

#ifdef HW_CRC32C

#if (defined(__amd64__) || defined(__x86_64__) || defined(_M_X64) || defined(_M_AMD64)) && defined(__SSE__)
#include <smmintrin.h>
#define HW_CRC32C_AVAILABLE
#elif defined(__aarch64__) && defined(__ARM_FEATURE_CRC32)
#include <arm_acle.h>
#define _mm_crc32_u8(a,b) __crc32cb(a,b)
#define _mm_crc32_u16(a,b) __crc32ch(a,b)
#define _mm_crc32_u32(a,b) __crc32cw(a,b)
#define _mm_crc32_u64(a,b) __crc32cd(a,b)
#define HW_CRC32C_AVAILABLE
#elif defined(_M_ARM64)
#include <intrin.h>
#define _mm_crc32_u8(a,b) __crc32cb(a,b)
#define _mm_crc32_u16(a,b) __crc32ch(a,b)
#define _mm_crc32_u32(a,b) __crc32cw(a,b)
#define _mm_crc32_u64(a,b) __crc32cd(a,b)
#define HW_CRC32C_AVAILABLE
#endif

#endif

#ifndef HW_CRC32C_AVAILABLE
#pragma "crc32c instruction set is unavailable or disabled on this platform"

#if __BYTE_ORDER__ != __ORDER_LITTLE_ENDIAN__
#error "big-endian software version not implemented"
#endif

#include "table.h"

static inline crc32c_t
_mm_crc32_u8(uint32_t crc, uint8_t v)
{
	return crc32c_lut[0][0xFF & (crc ^ v)] ^ (crc >> 8);
}

static inline crc32c_t
_mm_crc32_u16(uint32_t crc, uint16_t v)
{
	uint32_t i = v ^ crc;
	crc =
		crc32c_lut[1][(i >> 0) & 0xff] ^
		crc32c_lut[0][(i >> 8) & 0xff] ^
		(i >> 16);
	return crc;
}

static inline crc32c_t
_mm_crc32_u32(uint32_t crc, uint32_t v)
{
	v ^= crc;
	crc =
		crc32c_lut[3][(v >>  0) & 0xff] ^
		crc32c_lut[2][(v >>  8) & 0xff] ^
		crc32c_lut[1][(v >> 16) & 0xff] ^
		crc32c_lut[0][(v >> 24) & 0xff];
	return crc;
}

static inline crc32c_t
_mm_crc32_u64(uint32_t crc, uint64_t v)
{
	v ^= (uint64_t)crc;
	crc =
		crc32c_lut[7][(v >>  0) & 0xff] ^
		crc32c_lut[6][(v >>  8) & 0xff] ^
		crc32c_lut[5][(v >> 16) & 0xff] ^
		crc32c_lut[4][(v >> 24) & 0xff] ^
		crc32c_lut[3][(v >> 32) & 0xff] ^
		crc32c_lut[2][(v >> 40) & 0xff] ^
		crc32c_lut[1][(v >> 48) & 0xff] ^
		crc32c_lut[0][(v >> 56) & 0xff];
	return crc;
}
#endif

int
crc32c_hwaccel()
{
#ifdef HW_CRC32C_AVAILABLE
	return 1;
#else
	return 0;
#endif
}

inline crc32c_t
crc32c_update(crc32c_t crc, const void *buf, size_t length)
{
	const uint8_t *ptr = buf;
	size_t toalign = ((uintptr_t)ptr) & 7;

	while (length && toalign > 6) {
		crc = _mm_crc32_u8(crc, *ptr);
		ptr++; toalign--; length--;
	}
	if (length && toalign > 4) {
		crc = _mm_crc32_u16(crc, *(const uint16_t *)ptr);
		ptr += 2; toalign -= 2; length -= 2;
	}
	if (length && toalign) {
		crc = _mm_crc32_u32(crc, *(const uint32_t *)ptr);
		ptr += 4; toalign -= 4; length -= 4;
	}

	while (length >= 8) {
		crc = _mm_crc32_u64(crc, *(const uint64_t *)ptr);
		ptr += 8; length -= 8;
	}

	if (length & 4) {
		crc = _mm_crc32_u32(crc, *(const uint32_t *)ptr);
		ptr += 4; length -= 4;
	}
	if (length & 2) {
		crc = _mm_crc32_u16(crc, *(const uint16_t *)ptr);
		ptr += 2; length -= 2;
	}
	if (length & 1) {
		crc = _mm_crc32_u8(crc, *ptr);
		ptr++;
	}
	return crc;
}
