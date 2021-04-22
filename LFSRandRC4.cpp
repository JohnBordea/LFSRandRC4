#include <iostream>
#include <NTL/ZZ.h>

using namespace NTL;



int lfsrShiftTest4(int N) {
    N += ((((N & 1) + ((N >> 3) & 1)) % 2) << 4);
    return N >> 1;
}

unsigned int lfsrShift32(unsigned int N) {
    bool s_j = ((N & 1) + ((N >> 5/*27*/) & 1) + ((N >> 4/*28*/) & 1) + ((N >> 31) & 1)) % 2;
    N >>= 1;
    N += (s_j << 31);
    return N;
}

unsigned long long lfsrShift64(unsigned long long N) {
    bool s_j = ((N & 1) + ((N >> 60/*4*/) & 1) + ((N >> 61/*3*/) & 1) +((N >> 63) & 1)) % 2;
    N >>= 1;
    N += ((long long)s_j << 63);
    return N;
}

unsigned int calulatePeriodOfLfsrShift32() {
    unsigned int n, i = 0, k;
    k = n = 4;

    do {
        n = lfsrShift32(n);
        i++;
    } while (n != k);

    return i;
}

unsigned long long calulatePeriodOfLfsrShift64() {
    unsigned long long n, i = 0, k;
    k = n = 4;

    do {
        n = lfsrShift64(n);
        i++;
    } while (n != k);

    return i;
}

ZZ generateNumberLFSR(unsigned int bitLength) {
    ZZ randomNumber = (ZZ)0;

    unsigned long long seed = ((((((static_cast<unsigned long long>(RandomBits_long(16)) << 16) + RandomBits_long(16)) << 16) + RandomBits_long(16)) << 16) + RandomBits_long(16));

    //seed = 1;

    for (unsigned int i = 0; i < bitLength; i++) {
        randomNumber <<= 1;
        seed = lfsrShift64(seed);
        randomNumber += (seed >> 63);
    }

    return randomNumber;
}

int main()
{
    for (int i = 0; i < 32; i++)
        std::cout << generateNumberLFSR(1024) << std::endl;

    return 0;
}