#include <iostream>
#include <NTL/ZZ.h>

using namespace NTL;

int main()
{
    ZZ p;
    p = (ZZ)5000000;
    for (int i = 0; i < 10; i++) {
        p *= p;
        std::cout << p << "\n";
    }
}