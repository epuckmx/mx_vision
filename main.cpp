#include <iostream>
#include <fstream>
#include <string>
#include "mx_vision.h"

unsigned char image[BUFFER_SIZE];
std::string files[] = {
    "/data/aff2957cd4f5be4f5ac539724456b2e465859c097d068b32e08c0927641483df",
    "/data/83f55ddcc260177b10ad123d56a52bb63e25464e0b7b1e03490dc46b422607f2",
    "/data/ce5903e2cfd52b4241f03e0665e9fff081d7ca104e438f70ccf34b2c05c58dad"
};

int main() {
    int my_reds = 0;
    mx_vision_init();
    for (int i = 0; i < 3; i++) {
        std::ifstream file(files[i].c_str());
        if (file.is_open()) {
            int byte = 0;
            int counter = 0;
            while (file >> byte) {
                unsigned char charByte = (unsigned char)byte;
                image[counter] = charByte;
                counter++;
                if (counter == BUFFER_SIZE) {
                    mx_vision_see(image);
                    my_reds += redsDetected;
                    counter = 0;
                }
            }
        } else {
            std::cerr << "Error opening file" << std::endl;
        }
        file.close();
    }
    std::ofstream json("/workspace/results.json", std::ofstream::out);
    json << "{ overall: { metric: 'count', value: " << my_reds << " } }" << std::endl;
    json.close();
    return 0;
}
