#include "CGOL.h"
#include <iostream>
#include <chrono>

void prt(std::vector<PixelRGBA> mat, int x, int y) {
    for (int i = 0; i < y; i++) {
        for (int j = 0; j < x; j++) {
            std::cout << (mat[i * x + j].r == 255 ? "1" : "0");
        }
        std::cout << std::endl;
    }
}

int main() {
    struct {
        int x=1 << 11;
        int y=1 << 11;
    } simulationSize;
    CGOL c(simulationSize.x, simulationSize.y);
    c.Randomize();
    c.Start();
//    auto start = std::chrono::high_resolution_clock::now();
//    for(int i = 0;i<100000;i++){
//        c.Step();
//    }
//    auto end = std::chrono::high_resolution_clock::now();
//
//    // Calculate the duration in seconds
//    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
//    long long miliseconds = duration.count();
//
//    // Output the time taken
//    if (miliseconds>1000)
//        std::cout << "Time taken: " << miliseconds / 1000.0 << " seconds" << std::endl;
//    else
//        std::cout << "Time taken: " << miliseconds << " miliseconds3" << std::endl;

}