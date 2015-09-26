#include <stdio.h>
#include "mx_vision.h"

int main() {
	printf("First test\n");
	printf("---------------------\n");
	mx_vision_init(0);
	mx_vision_see();
	printf("Second test\n");
	printf("---------------------\n");
	mx_vision_init(1);
	mx_vision_see();
	printf("Third test\n");
	printf("---------------------\n");
	mx_vision_init(2);
	mx_vision_see();
	printf("Fourth test\n");
	printf("---------------------\n");
	mx_vision_init(3);
	mx_vision_see();	
	printf("Fifth test\n");
	printf("---------------------\n");
	mx_vision_init(4);
	mx_vision_see();
	return 0;
}