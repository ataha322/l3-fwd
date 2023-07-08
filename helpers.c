#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

static void uint32_to_str_big_endian(uint32_t num) {

	uint8_t byte1, byte2, byte3, byte4;
	
	byte1 = (num << 24) >> 24;
	byte2 = (num << 16) >> 24;
	byte3 = (num << 8) >> 24;
	byte4 = num >> 24;

	printf("%d.%d.%d.%d\n", byte1, byte2, byte3, byte4);
}


