//#include <Windows.h>
#include "../include/beacon.h"

void print(const char* str)
{
	BeaconPrintf(CALLBACK_OUTPUT, str);
}

void go(unsigned char* argdata, int argsize)
{
	print("Hello world!\n");
}