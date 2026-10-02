#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/crc32c.h"

typedef struct {
	crc32c_t 	crc;
	char 		str[25];
} crc32c_test;

const crc32c_test test[] = {
	{0x8284e2c8, "foobarbaz1234567"},
	{0x27f81851, "#foobarbaz1234567"},
	{0xcd474370, "##foobarbaz1234567"},
	{0x1f7bf1f6, "###foobarbaz1234567"},
	{0xbbfe11d4, "####foobarbaz1234567"},
	{0x7adc6a1d, "#####foobarbaz1234567"},
	{0xc122b56e, "######foobarbaz1234567"},
	{0xa351ec48, "#######foobarbaz1234567"},
	{0xa52ff627, "########foobarbaz1234567"},
};

int
main()
{
	int result = 0;
	size_t i;
	for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
		crc32c_t crc = crc32c(test[i].str, strlen(test[i].str));
		if (crc != test[i].crc) {	
			printf("%08" PRIx32 " differs from expected (%08" PRIx32 ")\t\tFAILED\n", crc, test[i].crc);
			result = 1;
			continue;
		}
	}

	printf("%s\n", (const char *[]){"All tests OK","One or more tests FAILED"}[result]);

	return result;
}
