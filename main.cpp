#include <iostream>
#include <fstream>
#include "mx_vision.h"

unsigned char image[BUFFER_SIZE];

int main() {
    mx_vision_init();
    std::string files[] = {"record1", "record2", "record3"};
    for (int i = 0; i < 3; i++) {
        mx_vision_reset();
        std::cout << "---" << files[i] << std::endl;
        std::ifstream file(files[i]);
        int byte = 0;
        int counter = 0;
        while (file >> byte) {
            unsigned char charByte = (unsigned char)byte;
            image[counter] = charByte;
            counter++;
            if (counter == BUFFER_SIZE) {
                mx_vision_see(image);
                std::cout << "[";
                for (int i = 0; i < redsDetected; ++i) {
                    std::cout << "(" << reds[i].id << "," << reds[i].dis << "," << reds[i].dir << ")";
                }
                std::cout << "],[";
                for (int i = 0; i < bluesDetected; ++i) {
                    std::cout << "(" << blues[i].id << "," << blues[i].dis << "," << blues[i].dir << ")";
                }
                counter = 0;
                std::cout << "]" << std::endl;
            }
        }
        file.close();
    }
    return 0;
}
