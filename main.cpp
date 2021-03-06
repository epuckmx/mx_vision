#include <iostream>
#include <fstream>
#include "mx_vision.h"

unsigned char image[BUFFER_SIZE];

int main() {
    mx_vision_init();
    std::ifstream file("record3");
    int byte = 0;
    int counter = 0;
    while (file >> byte) {
        unsigned char charByte = (unsigned char)byte;
        image[counter] = charByte;
        counter++;
        if (counter == BUFFER_SIZE) {
            mx_vision_see(image);
            std::cout << "Reds detected " << redsDetected << std::endl;
            for (int i = 0; i < redsDetected; ++i) {
                std::cout << "Object red at distance " << reds[i].dis << " direction " << reds[i].dir << std::endl;
            }
            std::cout << "Blues detected " << bluesDetected << std::endl;
            for (int i = 0; i < bluesDetected; ++i) {
                std::cout << "Object blue at distance " << blues[i].dis << " direction " << blues[i].dir << std::endl;
            }
            counter = 0;
        }
    }
    file.close();
    return 0;
}
