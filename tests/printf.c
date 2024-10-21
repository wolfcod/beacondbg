//#include <Windows.h>
#include "../include/beacon.h"

void go(unsigned char* argdata, int argsize)
{
	BeaconPrintf(CALLBACK_OUTPUT, "Hello world!\n");
}