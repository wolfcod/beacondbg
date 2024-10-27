#pragma once

struct bof_pack
{
	bof_pack();
	~bof_pack();

	void encodeShort(short value);
	void encodeInteger(int value);
	void encodeString(const char* str);
	void encodeWideString(const char* str);
	bool encodeBinary(const char* inputFile);

	void append(void* src, size_t size);
	
	char* bof;
	int bof_size;
};