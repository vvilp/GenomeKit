#include <iostream>
#include <vector>

using namespace std;

class Bit {
public:

    static void PrintBit(uint32_t hash) {
        for (int i = 0; i < sizeof(hash) * 8; ++i) {
            putc((hash & 1) ? '1' : '0', stdout);
            hash = hash >> 1;
        }
        putc('\n', stdout);
    }


    static int HammingDistance(uint32_t x, uint32_t y) {
        int dist = 0;
        unsigned c = x ^ y;
        c = c - ((c >> 1) & 0x55555555);
        c = (c & 0x33333333) + ((c >> 2) & 0x33333333);
        c = ((c + (c >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
        return c;
    }

};


class Hash {
private:

    enum HashMethod {
        FNV_32,
        FNV_64,
        FNV_128,
    };

    static const uint32_t FNV_PRIME_32 = 16777619;
    static const uint32_t FNV_OFFSET_32 = 2166136261U;

public:

    static uint32_t FNV32(const char *s) {
        uint32_t hash = FNV_OFFSET_32, i;
        for (i = 0; i < strlen(s); i++) {
            hash = hash ^ (s[i]);
            hash = hash * FNV_PRIME_32;
        }
        return hash;
    }
};


int main() {
    Bit::PrintBit(Hash::FNV32("AAAAAAA"));
    Bit::PrintBit(Hash::FNV32("AABCDAA"));
    Bit::PrintBit(Hash::FNV32("AAAAAAA"));
    Bit::PrintBit(Hash::FNV32("AAAABAA"));
    cout << Bit::HammingDistance(Hash::FNV32("AAAA"), Hash::FNV32("AAAV")) << endl;
    cout << Bit::HammingDistance(Hash::FNV32("AAAA"), Hash::FNV32("AAAC")) << endl;

//    Hash::GetHash("test", Hash::FNV_128, );
    return 0;
}