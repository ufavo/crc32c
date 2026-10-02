// Copyright (c) 2026 Luiz Gustavo S Borsoi
// Licensed under the MIT License. See LICENSE file in the project root.

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/crc32c.h"

static inline crc32c_t
fcrc32c(FILE *restrict f)
{
	uint8_t buf[4096];
	
	crc32c_t crc = crc32c_init();

	size_t read = 0;
	while (!feof(f)) {
		while (read < sizeof(buf)) {
			size_t r = fread(buf + read, 1, sizeof(buf), f);
			read += r;
			if (!r) goto done;
		}
		crc = crc32c_update(crc, buf, read);
		read = 0;
	}
done:
	if (read > 0)
		crc = crc32c_update(crc, buf, read);	
	
	return crc32c_finalize(crc);
}

static inline void
crc32c_files(char **filev, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		crc32c_t crc;
		char *name = filev[i];
	
		/* handle stdin */
		if (filev[i][0] == '-') {
			crc = fcrc32c(stdin);
			name = "(stdin)";
			goto print;
		}

		FILE *f = fopen(filev[i], "rb");
		if (!f) {
			fprintf(stderr, "Unable to open file: %s: %s\n", filev[i], strerror(errno));
			continue;
		}
		errno = 0;
		crc = fcrc32c(f);
		if (errno) {
			fprintf(stderr, "Error: %s: %s\n", name, strerror(errno));
			fclose(f);
			continue;
		}
		fclose(f);
print:
		printf("%08" PRIx32 "  %s\n", crc, name);
	}
}

int
main(int argc, char **argv)
{
	if (argc == 1) {
		fprintf(stderr, "Usage: %s [FILE | -]...\n", argv[0]);
		return 1;
	}
	crc32c_files(&argv[1], argc-1);
	return 0;
}
