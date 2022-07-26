// ./add-nbo.cpp

#include <stddef.h> // for size_t
#include <stdint.h> // for uint9_t
#include <stdio.h> // for printf
#include <netinet/in.h>

typedef struct _R{
	uint32_t data;
}R;

int main(int argc, char* argv[])
{
	if(argc != 3){
		printf("Please enter the path of two .bin files");
		return 1;
	}

	FILE *fp1;
	FILE *fp2;
	uint32_t origin1 = 0;
	uint32_t origin2 = 0;
	uint32_t one = 0;
	uint32_t two = 0;
	uint32_t buf = 0;
	uint32_t last = 0;
	uint32_t lastlast = 0;
	uint32_t first = 0;
	uint32_t second = 0;
	uint32_t third = 0;
	int carry_flag = 0;

	R * pointer1;
	R number1;
	pointer1=&number1;

	R * pointer2;
	R number2;
	pointer2=&number2;

	fp1 = fopen(argv[1], "rb");
	fp2 = fopen(argv[2], "rb");

	fread(pointer1, sizeof(uint32_t), 1, fp1);
	fread(pointer2, sizeof(uint32_t), 1, fp2);
	// now BE -> LE while being saved
	first = number1.data;
	second = number2.data;
	
	for(int k=0;k<4;k++){
		lastlast = lastlast << 8;

		origin1 = number1.data & 0xff000000;
		origin2 = number2.data & 0xff000000;
		number1.data = number1.data << 8;
		number2.data = number2.data << 8;

		if(origin1 == 0x00 && origin2 == 0x00){
			continue;
		}
	
		last = 0x00000000;

		for(int i=0;i<8;i++){
			last = last >> 1;

			one = origin1 & 0x01000000;
			two = origin2 & 0x01000000;
			origin1 = (origin1 >> 1) & 0xff000000;
			origin2 = (origin2 >> 1) & 0xff000000;
			
			if(carry_flag == 1){
				if((one == 0x01000000) && (two == 0x01000000)){
					carry_flag = 1;
					buf = 0x00000080;
				}
				else if((one == 0x01000000) || (two == 0x01000000)){
					carry_flag = 1;
					buf = 0x00000000;
				}
				else{
					carry_flag = 0;
					buf = 0x00000080;
				}
			}
			else{
				if((one == 0x01000000) && (two == 0x01000000)){
					carry_flag = 1;
					buf = 0x00000000;
				}
				else if((one == 0x01000000) || (two == 0x01000000)){
					carry_flag = 0;
					buf = 0x00000080;
				}
				else {
					carry_flag = 0;
					buf = 0x00000000;
				}
			}
			last = last | buf;
		}
		lastlast = lastlast | last;
	}
	first = htonl(first);
	second = htonl(second);
	third = htonl(lastlast);

	printf("%d(0x%x) + %d(0x%x) = %d(0x%x)", first, first, second, second, third, third);
	fclose(fp1);
	fclose(fp2);

	return 0;
}
