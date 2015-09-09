#include <stdio.h>
#include "mx_vision.h"

int main() {
	printf("First test\n");
	printf("---------------------\n");
	init(0);
	see();
	printf("Second test\n");
	printf("---------------------\n");
	init(1);
	see();
	printf("Third test\n");
	printf("---------------------\n");
	init(2);
	see();
	printf("Fourth test\n");
	printf("---------------------\n");
	init(3);
	see();	
	printf("Fifth test\n");
	printf("---------------------\n");
	init(4);
	see();
	return 0;
}