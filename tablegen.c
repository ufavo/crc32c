// Copyright (c) 2026 Luiz Gustavo S Borsoi
// Licensed under the MIT License. See LICENSE file in the project root.

#include <stdio.h>
#include <stdint.h>
#include <string.h>

uint32_t t[8][256];

int
main(void)
{
	printf("static const uint32_t crc32c_lut[8][256] = {\n\t{\n\t\t");

	memset(t, 0, sizeof(t));

	/* compute base table */
	uint32_t crc;
	int i, j;
	printf("0x00000000");
	for (i = 1; i < 256; i++) {
		printf(", ");
		if (i % 8 == 0) printf("\n\t\t");
		crc = i;
		for (j = 0; j < 8; j++)
			crc = (crc & 1) ? ((crc >> 1) ^ 0x82F63B78) : (crc >> 1);

		t[0][i] = crc;
		printf("0x%08X", crc);
	}
	printf("\n\t},");

	/* derive other tables */
	for (i = 1; i < 8; i++) {
		printf("{");
		for (j = 0; j < 256; j++) {
			crc = t[i-1][j];
			t[i][j] = t[0][crc & 0xFF] ^ (crc >> 8);
			
			if (j % 8 == 0) printf("\n\t\t");
			printf("0x%08X", t[i][j]);
			if (j+1 == 256) printf("\n");
			else printf(", ");
		}
		printf("\t}");
		if (i+1 < 8) printf(",");
	}

	printf("\n};");
	return 0;
}
