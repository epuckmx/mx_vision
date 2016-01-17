#include <iostream>
#include <fstream>
#include "mx_vision.h"

unsigned char image[BUFFER_SIZE];

int main() {
    mx_vision_init();
    std::ifstream file("record1");
    int byte = 0;
    int counter = 0;
    int checksum = 0;
    while (file >> byte) {
        unsigned char charByte = (unsigned char)byte;
        image[counter] = charByte;
        checksum += charByte;
        counter++;
        if (counter == BUFFER_SIZE) {
	    std::cout << "Check sum " << checksum << std::endl;
	    mx_vision_see(image);
	    std::cout << "Reds detected " << redsDetected << std::endl;
            counter = 0;
            checksum = 0;
            break;
        }
    }
    file.close();
    return 0;
}
