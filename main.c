#include <stdio.h>
#include "mx_vision.h"

int main() {
	printf("First test\n");
	printf("---------------------\n");
	mx_init(0);
	mx_see();
	printf("Second test\n");
	printf("---------------------\n");
	mx_init(1);
	mx_see();
	printf("Third test\n");
	printf("---------------------\n");
	mx_init(2);
	mx_see();
	printf("Fourth test\n");
	printf("---------------------\n");
	mx_init(3);
	mx_see();	
	printf("Fifth test\n");
	printf("---------------------\n");
	mx_init(4);
	mx_see();
	return 0;
}