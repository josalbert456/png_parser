#ifndef __MAIN_HEADER__
#define __MAIN_HEADER__

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TRUE						1
#define FALSE						0

#define CHUNK 		256

#define FATAL_ERROR(reason)			{printf("Fatal Error: %s\n", reason); exit(-1);}
#define WARNING(reason)				{printf("Warning: %s\n", reason);}
#define MILD_ERROR(reason, ifExit)	{printf("Mind Error: %s\n", reason); \
									if(ifExit) exit(-1);}

enum{IHDR, IDAT, IEND, tRNS, tIME, gAMA, 
	 cHRM, sBIT, sPLT, sRGB, iCCP, iTXt, 
	 tEXt, zTXt, bKGD, pHYs, hIST, UNKNOWN};

struct IHDR_info{
	unsigned int width;
	unsigned int height;
	unsigned char bit_depth;
	unsigned char color_type;
	unsigned char compress_method;
	unsigned char filter_method;
	unsigned char interlace_method;
};
struct IHDR_info info;

enum {GRAYSCALE, RGB, PALETTE, GRAYSCALE_ALPHA, RGB_ALPHA};

// check if one chunk is critical, public and unconform, unsafe 0x01 | 0x02
// if(chunk_info & 0x03);	 
#define CRITICAL_FLAG			0x01	// 0000 0001
#define PUBLIC_FLAG				0x02	// 0000 0010
#define CONFORM_FLAG			0x04	// 0000 0100
#define UNSAFE_FLAG				0x08	// 0000 1000	 
struct ChunkInfo{
	char chunk_type;
	char bit_5_flag;
	int chunk_len;
}chunk_info;
struct image_info{
	float cHRM[8];
}image_info;

#endif