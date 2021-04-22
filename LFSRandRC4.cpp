#include <iostream>
#include <NTL/ZZ.h>
#include <time.h>
#include <string>

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

ZZ generateNumberBBS(unsigned long bitLength) {
    ZZ p, q;
    do {
        do {
            p = GenPrime_ZZ(bitLength / 2);
        } while (p % 4 != 3);
        do {
            q = GenPrime_ZZ(bitLength / 2);
        } while (q % 4 != 3);
    } while (p == q);

    ZZ N = p * q;

    ZZ seed;
    do {
        seed = RandomBnd(N);
    } while (seed % p == 0 || seed % q == 0);

    ZZ randomNumber = (ZZ)0;

    for (int i = 0; i < bitLength; i++) {
        seed = (seed * seed) % N;
        randomNumber = (randomNumber << 1) + (seed % 2);
    }

    return randomNumber;
}

ZZ jacobiSymbol(ZZ n, ZZ k) {
    n %= k;
    int t = 1;
    while (n != 0) {
        while (n % 2 == 0) {
            n /= 2;
            int r = k % 8;
            if (r == 3 || r == 5)
                t = -t;
        }
        swap(n, k);
        if (n % 4 == 3 && k % 4 == 3)
            t = -t;
        n %= k;
    }
    return (k == 1) ? (ZZ)t : (ZZ)0;
}

ZZ generateNumberJacobi(unsigned long bitLength) {
    ZZ a, n;

    n = GenPrime_ZZ(bitLength);
    a = RandomBnd(n);

    ZZ randomNumber = (ZZ)0;

    for (int i = 0; i < bitLength; i++) {
        randomNumber <<= 1;
        if (jacobiSymbol(a + i, n) != (ZZ)-1)
            randomNumber += 1;
    }

    return randomNumber;
}

void testPercentage(int iterations) {
    int countZero = 0, countOne = 0, count;
    ZZ number;

    for (int i = 0; i < iterations; i++) {
        number = generateNumberLFSR(1024);
        while (number != 0) {
            switch (bit(number, 0) /*number % 2*/) {
            case 0:
                countZero++;
                break;
            case 1:
                countOne++;
                break;
            }
            number >>= 1;
        }
    }

    double percentage = 0;

    count = countOne + countZero;
    percentage = static_cast<double>(countZero) / static_cast<double>(count);
    std::cout << "Percentages of\n0: " << percentage * 100 << "%\n1: " << (1 - percentage) * 100 << "%\n";
}

int main(){
    int choice = 1;
    int choiceLFSR = -1;
    double timeLFSR, timeBBS, timeJacobi;
    time_t start, end;

    /*while (choice) {
        switch (choice) {
        case 1:
            std::cout << "1. LFSR\n2. RC4\n3. Exit\n";

            do {
                std::cin >> choice;
            } while (choice <= 0 || choice > 3);

            choice++;
            if (choice == 4)
                choice = 0;

            break;
        case 2:
            std::cout << "1. Generate Number With LFSR\n2. Compare Performance with other Generators\n3. Test Percentage\n4. Back\n";
            do {
                std::cin >> choiceLFSR;
            } while (choiceLFSR <= 0 || choiceLFSR > 4);

            switch (choiceLFSR) {
            case 1:
                std::cout << generateNumberLFSR(1024) << "\n";
                break;
            case 2:
                start = clock();
                for (int i = 0; i < 100; i++)
                    generateNumberLFSR(1024);
                end = clock();
                timeLFSR = ((double)(end - start) / double(CLOCKS_PER_SEC)) / 100;

                start = clock();
                for (int i = 0; i < 10; i++)
                    generateNumberBBS(1024);
                end = clock();
                timeBBS = ((double)(end - start) / double(CLOCKS_PER_SEC)) / 10;

                start = clock();
                for (int i = 0; i < 5; i++)
                    generateNumberJacobi(1024);
                end = clock();
                timeJacobi = ((double)(end - start) / double(CLOCKS_PER_SEC)) / 5;

                std::cout << "Time taken for LFSR => " << std::fixed << timeLFSR << "\n";
                std::cout << "Time taken for BBS => " << std::fixed << timeBBS << "\n";
                std::cout << "Time taken for Jacobi => " << std::fixed << timeJacobi << "\n";
                std::cout << "The Time was aproximated for one Iteration\n";

                break;
            case 3:
                testPercentage(100);
                break;
            case 4:
                choice = 1;
            }
            break;
        case 3:
            choice = 1;
            break;
        }
    }*/

    std::string key;

    key = "pedia";
    int S[256];
    int i, j;

    for (i = 0; i < 256; i++)
        S[i] = i;
    
    j = 0;

    for (i = 0; i < 256; i++) {
        j = (j + S[i] + key[i % key.size()]) % 256;
        swap(S[i], S[j]);
    }

    return 0;
}