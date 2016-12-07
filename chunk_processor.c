#include "main.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <zlib.h>

int retrive_integer_value(unsigned char *buffer){
	unsigned char g[] = {buffer[3], buffer[2], buffer[1], buffer[0]};
	unsigned int result;
	memcpy(&result, g, 4);
	return result;
}
unsigned short retrive_short_value(unsigned char *buffer){
	unsigned char value[2] = {buffer[1], buffer[0]};
	unsigned short result;
	memcpy(&result, value, 2);
	return result;
}
void IHDR_processor(unsigned char*buffer){
	printf("-----Process IHDR Chunk\n");
	buffer+=8;
	unsigned char len[4];

	int j = 0;	
	for(;j<4;j++) len[j]=buffer[3-j];
	int w = 0;
	memcpy(&w, len, 4);
	info.width = w;
	buffer+=4;
	
	j = 0;
	for(;j<4;j++) len[j]=buffer[3-j];	
	w = 0;
	memcpy(&w, len, 4);
	info.height = w;
	buffer+=4;
	
	info.bit_depth = *buffer;
	buffer++;

	switch(*buffer){
		case 0: info.color_type = GRAYSCALE;break;
		case 2: info.color_type = RGB;break;
		case 3: info.color_type = PALETTE; break;
		case 4: info.color_type = GRAYSCALE_ALPHA;break;
		case 6: info.color_type = RGB_ALPHA; break;
	}
	buffer++;

	if(*buffer!=0)FATAL_ERROR("Compression method unknown!");
	buffer++;

	if(*buffer!=0)FATAL_ERROR("Filter Method unknown");
}
void IDAT_processor(unsigned char*buffer){
	printf("-----Process IDAT chunk\n");
	buffer+=8;

	// 1, uncompress with zlib from source
	// 2, unfilter from uncompress data
}
void tRNS_processor(unsigned char*buffer){
	// no pics with tRNS set found, this implementation is not tested
	printf("-----Process tRNS data\n");
	buffer+=8;
	int length = chunk_info.chunk_len;
	switch(info.color_type){
		case PALETTE:
			while(length--)printf("Pallette sRNS: %d\n", *buffer);
			buffer++;
			break;
		case GRAYSCALE_ALPHA:
			printf("sRNS value: %d\n", retrive_short_value(buffer));
			break;
		case RGB_ALPHA:
			printf("sRNS Red: %d\n", retrive_short_value(buffer)); buffer+=2;
			printf("sRNS Green: %d\n", retrive_short_value(buffer)); buffer+=2;
			printf("sRNS Blue: %d\n", retrive_short_value(buffer));
			break;
	}
}	
void tIME_processor(unsigned char*buffer){
	printf("-----Processor tIME\n");
	printf("Time Info: \n");
	buffer+=8;
	unsigned short year = retrive_short_value(buffer);
	printf("\t%d %d %d\n", year, buffer[2], buffer[3]);
	printf("\t%d:%d:%d\n", buffer[4], buffer[5], buffer[6]);
}		
void gAMA_processor(unsigned char*buffer){
	printf("-----Process gAMA\n");
	buffer+=8;
	unsigned char g[] = {buffer[3], buffer[2], buffer[1], buffer[0]};
	unsigned int result;
	memcpy(&result, g, 4);
	printf("Gama Value: %d\n", result);
}
void cHRM_processor(unsigned char*buffer){
	printf("-----Process cHRM\n");
	buffer+=8;
	char *marker[8] = {"while point x: ", "white point y: ", "red x: ",
					"red y: ", "green x: ", "green y: ", "blue x: ", "blue y: "};

	int i=0; 
	for(; i<8; i++){
		int result = retrive_integer_value(buffer);
		float r = result/100000.f;
		printf("%s:%f\n", marker[i], r);
		buffer+=4;
	}
	printf("\n");
}
void sBIT_processor(unsigned char*buffer){
	printf("-----Process sBIT\n");
	buffer+=8;
	/*int i = 0;
	printf("Bit Info: \n");
	for(; i<chunk_info.chunk_len; i++){
		printf("\t%d\n", buffer[i]);
	}*/
	switch(info.color_type){
		case GRAYSCALE:printf("sBit: %d\n", *buffer);break;
		case RGB: case PALETTE: printf("sBit: %d(r) %d(g) %d(b)\n", buffer[0], buffer[1], buffer[2]);break;
		case GRAYSCALE_ALPHA: printf("sBit: %d(s) %d(a)\n", buffer[0], buffer[1]);
		case RGB_ALPHA: 
			printf("sBit: %d(r) %d(g) %d(b) %d(a)\n", buffer[0], buffer[1], buffer[2], buffer[3]);
			break;
	}
}
void sPLT_processor(unsigned char*buffer){
	// haven't found any Pallette pics, this functions is not tested
	printf("-----Process sPLT\n");
	buffer+=8;
	int pallette_len = chunk_info.chunk_len;
	printf("Pallette length: %d\n", pallette_len);
	int i=0;
	for(; i<pallette_len; i+=3){
		printf("Red: %d\n", buffer[i]);
		printf("Green: %d\n", buffer[i+1]);
		printf("Blue: %d\n", buffer[i+2]);
	}
}
void sRGB_processor(unsigned char*buffer){
	printf("-----Process sRGB\n");
	buffer+=8;
	switch(*buffer){
		case 0:printf("Rendering Intent: Perceptual\n"); break;
		case 1:printf("Rendering Intent: Relative colorimetric\n"); break;
		case 2:printf("Rendering Intent: Saturation\n"); break;
		case 3:printf("Rendering Intent: Absolute colorimetric\n"); break;
	}
}
void iCCP_processor(unsigned char*buffer){
	printf("-----Process iCCP\n");
	buffer+=8;
	int profile_length = chunk_info.chunk_len;
	while(1){
		if(*buffer==NULL)break;
		printf("%c", *buffer);
		buffer++;
		profile_length--;
	}
	printf("\n");
	buffer++;
	profile_length--;
	printf("ICCP_Method: %d\n", *buffer);
	printf("Compression Profile Length: %d\n", profile_length);
	// iccp is compressed as zlib stream
	// next is to decompress the stream to 
	// icc profile 	
}
void iTXt_processor(unsigned char*buffer){
	printf("-----Process iTXt\n");
}
void tEXt_processor(unsigned char*buffer){
	printf("-----Process tEXt\n");
	//printf("%d\n", chunk_info.chunk_len);
	int i=0;
	buffer+=8;
	for(; i<chunk_info.chunk_len;i++){
		if(buffer[i]==NULL)printf(":");
		else printf("%c", buffer[i]);
	}
	printf("\n");
}
void zEXt_processor(unsigned char*buffer){
	printf("-----Process zEXt\n");
}

void bKGD_processor(unsigned char*buffer){
	printf("-----Process bKGD\n");
	buffer+=8;
	unsigned short result;
	switch(info.color_type){
		case GRAYSCALE:case GRAYSCALE_ALPHA:
			result = retrive_short_value(buffer);
			printf("Range 1: %d\n", result); buffer+=2;
			result = retrive_short_value(buffer);
			printf("Range 2: %d\n", result);
			break;
		case RGB:case RGB_ALPHA:
			result = retrive_short_value(buffer);
			printf("Red: %d\n", result); buffer+=2;
			result = retrive_short_value(buffer);
			printf("Green: %d\n", result);buffer+=2;
			result = retrive_short_value(buffer);
			printf("Blue: %d\n", result);
			break;
		case PALETTE:
			result = retrive_short_value(buffer);
			printf("Pallette Index: %d\n", result);
			break;
	}
}
void pHYs_processor(unsigned char*buffer){
	printf("-----Process pHYs\n");
	buffer+=8;
	int x_unit = retrive_integer_value(buffer);buffer+=4;
	int y_unit = retrive_integer_value(buffer);buffer++;
	int spec_unit = *buffer;
	printf("X Unit: %d\n", x_unit);
	printf("Y Unit: %d\n", y_unit);
	printf("Spec Unit: %d\n", spec_unit);
}
void hIST_processor(unsigned char*buffer){
	printf("-----Process hIST\n");
}
void UNKNOWN_processor(unsigned char*buffer){
	printf("-----Process UNKNOWN\n");
}
