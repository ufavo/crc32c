// Copyright (c) 2026 Luiz Gustavo S Borsoi
// Licensed under the MIT License. See LICENSE file in the project root.

#ifndef __crc32c_h__
#define __crc32c_h__

#include <stdint.h>
#include <stddef.h>

typedef uint32_t crc32c_t;

#define crc32c_init() (~(crc32c_t)0)
#define crc32c_finalize(crc) (~(crc))

crc32c_t 	crc32c_update(crc32c_t crc, const void *buf, size_t length);
int 		crc32c_hwaccel();

static inline crc32c_t
crc32c(const void *buf, size_t length)
{
	return crc32c_finalize(crc32c_update(crc32c_init(), buf, length));
}

#endif
