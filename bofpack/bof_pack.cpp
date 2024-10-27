#include <stdint.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <fstream>
#include <format>
#include <vector>

#include "bof_pack.h"

template<typename T>
T bswap(T value)
{
	T r = 0;

	for (int i = 0; i < sizeof(T); i++) {
		uint8_t n = (uint8_t)value & 0xff;
		r |= n << (sizeof(T) - i - 1) * 8;
	}

	return r;
}

bof_pack::bof_pack()
	: bof(nullptr), bof_size(0)
{

}
bof_pack::~bof_pack()
{
	if (bof != nullptr)
		free(bof);
	bof = nullptr;
	bof_size = 0;
}

void bof_pack::append(void* src, size_t size)
{
	char* tmp = (char *)realloc(bof, bof_size + size);
	if (tmp != nullptr) {
		memcpy(tmp + bof_size, src, size);
		bof_size += size;
		bof = tmp;
	}
}
void bof_pack::encodeShort(short value)
{
	short n = bswap<short>(value);

	append(&n, sizeof(short));
}

void bof_pack::encodeInteger(int value)
{
	int n = bswap<int>(value);

	append(&n, sizeof(int));
}

void bof_pack::encodeString(const char* str)
{
	encodeInteger(strlen(str) + 1);
	append((void *)str, strlen(str) + 1);
}

void bof_pack::encodeWideString(const char* str)
{
	char dummy[2] = {};

	encodeInteger((strlen(str) + 1) * 2);

	for (int i = 0; i < strlen(str); i++) {
		dummy[0] = str[i]; dummy[1] = 0;
		append(dummy, 2);
	}
	dummy[0] = 0; dummy[1] = 0;
	append(dummy, 2);
}

bool bof_pack::encodeBinary(const char* inputFile)
{
	std::ifstream input(inputFile, std::ios::binary);

	if (!input) {
		return false;
	}

	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	encodeInteger((int)buffer.size());
	append(buffer.data(), buffer.size());
	return true;
}
