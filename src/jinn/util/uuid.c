
#include "uuid.h"

static void add(unsigned char* bytes, int offset, unsigned long value)
{
	int i;
	for (i = 0; i < 4; ++i)
	{
		bytes[offset++] = (unsigned char)(value & 0xff);
		value >>= 8;
	}
}

static unsigned long do_hash(const char* str, int seed)
{
	/* DJB2 hashing; see http://www.cse.yorku.ca/~oz/hash.html */
	
	unsigned long hash = 5381;
	
	if (seed != 0) {
		hash = hash * 33 + seed;
	}

	while (*str) {
		hash = hash * 33 + (*str);
		str++;
	}

	return hash;
}

void hash_uuid(const char* name, unsigned char* bytes /* 16 */) {
	if (name)
	{
		add(bytes, 0, do_hash(name, 0));
		add(bytes, 4, do_hash(name, 'L'));
		add(bytes, 8, do_hash(name, 'u'));
		add(bytes, 12, do_hash(name, 'a'));
	}
}

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>

void hash_uuid_str(const char* in, char* uuid /* 38 */) {
	unsigned char bytes[16];
	hash_uuid(in, bytes);
	sprintf(uuid, "%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X",
		bytes[0], bytes[1], bytes[2], bytes[3],
		bytes[4], bytes[5],
		bytes[6], bytes[7],
		bytes[8], bytes[9],
		bytes[10], bytes[11], bytes[12], bytes[13], bytes[14], bytes[15]);
}


