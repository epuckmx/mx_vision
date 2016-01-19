#include <iostream>
#include <fstream>
#include "mx_vision.h"

unsigned char image[BUFFER_SIZE];

int main() {
    mx_vision_init();
    std::ifstream file("record1");
    int byte = 0;
    int counter = 0;
    while (file >> byte) {
        unsigned char charByte = (unsigned char)byte;
        image[counter] = charByte;
        counter++;
        if (counter == BUFFER_SIZE) {
            mx_vision_see(image);
            std::cout << "Reds detected " << redsDetected << std::endl;
            if (redsDetected > 0) {
                std::cout << "At distance " << red.dis << " and direction " << red.dir << std::endl;
            }
            counter = 0;
        }
    }
    file.close();
    return 0;
}
