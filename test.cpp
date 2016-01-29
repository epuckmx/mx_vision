#include <iostream>
#include "mx_vision.h"
#include "mx_vision_test.h"

int main() {
	mx_vision_init();
	mx_vision_set(test0);
	unsigned char r, g, b;
	getColorR(0, 0, &r);
	getColorG(0, 0, &g);
	getColorB(0, 0, &b);
	std::cout << "Red " << (int)r << std::endl;
	std::cout << "Green " << (int)g << std::endl;
	std::cout << "Blue " << (int)b << std::endl;
	setColorR(0, 0, 112);
	getColorR(0, 0, &r);
	std::cout << "Red " << (int)r << std::endl;
	return 0;
}