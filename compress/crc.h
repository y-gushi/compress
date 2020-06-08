#pragma once
#include "typechange.h"

class crc {
public:
    UINT32 crc32;
    UINT32 magic;
    UINT32 table[256] = { 0 };
    UINT32 tablev;
    crc();
    unsigned int mcrc(unsigned char* a, UINT64 len);
    void rescrc();
};