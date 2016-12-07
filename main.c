#include "main.h"
#include "chunk_processor.h"

int retreive_chunk_length(unsigned char *buffer){
	int chunk_len = 0;
	unsigned char len [] = {buffer[3], buffer[2], buffer[1], buffer[0]};
	memcpy(&chunk_len, len, 4);
	unsigned char *entire_chunk = malloc(sizeof(char) * (chunk_len + 12));
	memset(entire_chunk, 0, chunk_len + 12);	
	return chunk_len;
}
void retrieve_chunk_type(unsigned char *buffer){
	//buffer+=4;
	if(buffer[7]=='T'){
		switch(buffer[4]){
			case 'I':chunk_info.chunk_type = IDAT;break;
			case 's':
				if(buffer[5]=='B') chunk_info.chunk_type = sBIT; 
				else if(buffer[5]=='P')chunk_info.chunk_type = sPLT; 
				break;
			case 'h':chunk_info.chunk_type = hIST; break;
		}
	}else if(buffer[7]=='t'){
		switch(buffer[4]){
			case 'i':chunk_info.chunk_type = iTXt; break;
			case 't':chunk_info.chunk_type = tEXt; break;
			case 'z':chunk_info.chunk_type = zTXt; break;
		}
	}else if(buffer[4]=='I'){
		if(buffer[5]=='H')chunk_info.chunk_type = IHDR;
		else if(buffer[5]=='E') chunk_info.chunk_type = IEND;
	}else if(buffer[4]=='t'){
		if(buffer[5]=='R')chunk_info.chunk_type = tRNS;
		else if(buffer[5]=='I')chunk_info.chunk_type = tIME;
	}else{
		switch(buffer[4]){
			case 'g':chunk_info.chunk_type = gAMA; break;
			case 'b':chunk_info.chunk_type = bKGD; break;
			case 'c':chunk_info.chunk_type = cHRM; break;
			case 'i':chunk_info.chunk_type = iCCP; break;
			case 'p':chunk_info.chunk_type = pHYs; break;
			case 'U':chunk_info.chunk_type = UNKNOWN; break;
			case 's':chunk_info.chunk_type = sRGB; break;
		}
	}

	if(!(buffer[4] & 32)) chunk_info.bit_5_flag |= CRITICAL_FLAG;
	if(!(buffer[5] & 32)) chunk_info.bit_5_flag |= PUBLIC_FLAG; 
	if(!(buffer[6] & 32)) chunk_info.bit_5_flag |= CONFORM_FLAG;
	if(!(buffer[7] & 32)) chunk_info.bit_5_flag |= UNSAFE_FLAG;
}
void process_chunk_data(char chunk_type, unsigned char *buffer){
	switch(chunk_type){
		case IHDR: IHDR_processor(buffer); break;
		case IDAT: IDAT_processor(buffer); break;
		case IEND: break;
		case tRNS: tRNS_processor(buffer); break;
		case tIME: tIME_processor(buffer);break;
		case gAMA: gAMA_processor(buffer);break;
		case cHRM: cHRM_processor(buffer);break;
		case sBIT: sBIT_processor(buffer);break;
		case sPLT: sPLT_processor(buffer);break;
		case sRGB: sRGB_processor(buffer);break;
		case iCCP: iCCP_processor(buffer);break;
		case iTXt: iTXt_processor(buffer);break;
		case tEXt: tEXt_processor(buffer);break;
		case zTXt: zEXt_processor(buffer);break;
		case bKGD: bKGD_processor(buffer);break;
		case pHYs: pHYs_processor(buffer);break;
		case hIST: hIST_processor(buffer);break;
		case UNKNOWN: UNKNOWN_processor(buffer);break;
	}
}
void retrieve_chunk_info(unsigned char *buffer){
	// chunk data length
	int i= 0;
	for(; i<10; i++){
		// chunk length	
		chunk_info.chunk_len = retreive_chunk_length(buffer);

		// chunk type
		retrieve_chunk_type(buffer);
		
		printf("%c%c%c%c: ", buffer[4], buffer[5], buffer[6], buffer[7]);
		printf("%d\n", chunk_info.chunk_len);
		// chunk data
		process_chunk_data(chunk_info.chunk_type, buffer);
		if(chunk_info.chunk_len==0)break;		
		// crc

		// process foward
		buffer += (chunk_info.chunk_len + 12);		
	}
}
char checkFileType(unsigned char *buffer){
	unsigned char header[] = {0x76, 0xaf, 0xb1, 0xb8, 0xf2, 0xf5, 0xe5, 0xf5};
	int i= 0; 

	for(i=0; i<8; i++){
		if((*buffer)&header[i])break;
		else buffer++;
	}
	if(i==8){
		return TRUE;	
	}else{
		return FALSE;
	}
}
void pro_func(unsigned char *buffer){
	char type = checkFileType(buffer);
	buffer += 8;
	if(type){
		retrieve_chunk_info(buffer);		
	}else{
		FATAL_ERROR("Not PNG File");
	}
}
void process_files(int argc, char **argv, void (*pro_func)(unsigned char *buffer)){
	int i;
	for(i=1; i<argc; i++){
		FILE *file = fopen(argv[i], "r+");
		fseek(file, 0, SEEK_END);
		long len = ftell(file);
		printf("%s: \n\tFile Length: %ld\n", argv[i], len);
		char *buffer = (char *)malloc(sizeof(char)*len);
		memset(buffer, 0, len);
		rewind(file);
		fread(buffer, len, 1, file);
		pro_func(buffer);
		fclose(file);	
		//enum {GRAYSCALE, RGB, PALETTE, GRAYSCALE_ALPHA, RGB_ALPHA};
		printf("\nwidth: %d, height: %d\n", info.width, info.height);
		printf("bit_depth: %d\n", info.bit_depth);
		printf("color_type: ");
		switch(info.color_type){
			case GRAYSCALE: printf("Grayscale\n"); break;
			case RGB: printf("RGB\n"); break;
			case PALETTE: printf("Palette\n"); break;
			case GRAYSCALE_ALPHA: printf("Grayscale-Alpha\n"); break;
			case RGB_ALPHA: printf("RGB-Alpha\n"); break;
		}
		printf("interlace_method: %d\n", info.interlace_method);
		printf("\n");
	}	
}
int  main(int argc, char **argv){
	process_files(argc, argv, pro_func);
}