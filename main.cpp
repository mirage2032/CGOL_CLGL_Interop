#include "CGOL.h"
#include <iostream>

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
        int x=1920;
        int y=1080;
    } sz;
    CGOL c(sz.x, sz.y);
    c.Randomize();
//    auto x = c.Read();
//    prt(x,sz.x, sz.y);
//    std::cout << "-------------\n-------------"<<std::endl;
//    c.Step();
//    x = c.Read();
//    prt(x,sz.x, sz.y);
//    std::cout << "-------------\n-------------"<<std::endl;
    for(int i = 0;i<10000;i++){
        c.Step();
    }
    std::cout<<3;
//    x = c.Read();
//    prt(x,sz.x, sz.y);
//    c.Start();
//    c.Step();
//    x = c.Read();
//    prt(x,10,6);
}